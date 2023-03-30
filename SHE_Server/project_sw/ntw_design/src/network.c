#include "network.h"

extern volatile int dhcp_timoutcntr;
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
struct netif server_netif;
struct tcp_pcb *pcb;
void tcp_fasttmr(void);
void tcp_slowtmr(void);
void lwip_init();
err_t dhcp_start(struct netif *netif);

volatile uint32_t * SHAREDMEM;

int packet_count; // Number of packets send/recieved for current transmission
int sending; // 0=do not send, 1=send
int state; // 0=relin, 1=in_cipher, 2=out_cipher

char *relin_pointer;
char *ctin_pointer;
char *ctout_pointer;

int core; // The core which is being used
extern IN_CIPHERTEXT in_cipher0;
extern IN_CIPHERTEXT in_cipher1;
extern OUT_CIPHERTEXT out_cipher0;
extern OUT_CIPHERTEXT out_cipher1;
extern RLK_CONSTANTS rlkeys;

void reset_pointers()
{
	if(core == 0)
	{
		ctin_pointer = (char*)(in_cipher0.c00[0]);
		ctout_pointer = (char*)(out_cipher0.c0[0]);
	}
	else
	{
		ctin_pointer = (char*)(in_cipher1.c00[0]);
		ctout_pointer = (char*)(out_cipher1.c0[0]);
	}
	relin_pointer = (char*)rlkeys.rlk00[0];
}

void select_core()
{
	if(core == 0)
	{
		SHAREDMEM[APP_SEL_0] = 1;
		SHAREDMEM[APP_SEL_1] = 0;
	}
	else
	{
		SHAREDMEM[APP_SEL_0] = 0;
		SHAREDMEM[APP_SEL_1] = 1;
	}
}

void init_network(int core_select, volatile uint32_t *shared_memory)
{
	packet_count = 0;
	sending = 0;
	state = 0;
	core = core_select;
	SHAREDMEM = shared_memory;
	select_core();
	reset_pointers();
}

void print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void start_calculation()
{
	SHAREDMEM[APP_KEY] = 1;
	sending = 1;
	xil_printf("Calculating...\n\r");
}

err_t send_callback(void* arg, struct tcp_pcb *tpcb, u16_t len)
{
	if (state == 2)
	{
		if (tcp_sndbuf(pcb) != TCP_SND_BUF)
		{
			// Empty the send buffer first!
			return ERR_OK;
		}
		if ((packet_count+1) * (TCP_SND_BUF) > sizeof(OUT_CIPHERTEXT))
		{
			// Last package
			int length = sizeof(OUT_CIPHERTEXT) - (packet_count * (TCP_SND_BUF));
			err_t err = tcp_write(tpcb, ctout_pointer, length, 1);
			if (err != ERR_OK)
			{
				xil_printf("Error %d while writing data\n\r", err);
				return 1;
			}
			// out_ciphertext fully send
			packet_count = 0;
			reset_pointers();
			state = 1;
			xil_printf("Result ciphertext send.\n\r");
			return ERR_OK;
		}
		else
		{
			err_t err = tcp_write(tpcb, ctout_pointer, TCP_SND_BUF, 1);
			if (err != ERR_OK)
			{
				xil_printf("Error %d while writing data\n\r", err);
				return 1;
			}
			ctout_pointer = ctout_pointer + TCP_SND_BUF;
			packet_count = packet_count + 1;
		}
	}
	return ERR_OK;
}

void send_data()
{
	if (SHAREDMEM[APP_KEY] == 0)
	{
		sending = 0;
		packet_count = 0; // Reset packet_count, just to be sure
		send_callback(NULL, pcb, 0);
	}
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *pbuf, err_t err)
{
	/* Do not read the packet if we are not in ESTABLISHED state */
	if (!pbuf) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	int length = pbuf->len;
	tcp_recved(tpcb, length); // Sending ACK

	// If 0, we want to recieving a relin key, else if 1 we want to recieve a in_ciphertext
	if(state == 0)
	{
		char *val = (char*)pbuf->payload;
		for (int i = 0; i < length; i++)
		{
			relin_pointer[i] = val[i];
		}
		relin_pointer = relin_pointer + length;
		if (relin_pointer == ((char*)&rlkeys) + sizeof(RLK_CONSTANTS))
		{
			// Relinkey fully recievedSH
			reset_pointers();
			state = 1;
			xil_printf("Relinearisation keys recieved.\n\r");
		}
	}
	else if(state == 1)
	{
		char *val = (char*)(pbuf->payload);
		for (int i = 0; i < length; i++)
		{
			ctin_pointer[i] = val[i];
		}
		ctin_pointer = ctin_pointer + length;
		if ((core == 0 && ctin_pointer == (char*)&in_cipher0 + sizeof(IN_CIPHERTEXT)) ||
			(core == 1 && ctin_pointer == (char*)&in_cipher1 + sizeof(IN_CIPHERTEXT)))
		{
			reset_pointers();
			state = 2;
			xil_printf("Ciphertexts recieved.\n\r");
			start_calculation();
		}
	}
	pbuf_free(pbuf);
	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	xil_printf("Connection established.\n\r");
	pcb = tpcb;
	tcp_sent(pcb, send_callback);
	tcp_recv(pcb, recv_callback);
	tcp_arg(pcb, NULL);
	return ERR_OK;
}

void configure_callbacks()
{
	err_t err;
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
	}
	err = tcp_bind(pcb, IP_ANY_TYPE, PORT);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", PORT, err);
	}
	tcp_arg(pcb, NULL);
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
	}
	tcp_accept(pcb, accept_callback);
	xil_printf("TCP server started @ port %d\n\r", PORT);
}

int start_network()
{
	ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_ethernet_address[] = MAC_ADDRESS;
    ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;

	lwip_init();
	if (!xemac_add(&server_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR))
	{
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
	netif_set_default(&server_netif);
	platform_enable_interrupts();
	netif_set_up(&server_netif);
	dhcp_start(&server_netif);
	dhcp_timoutcntr = 24;

	while(((server_netif.ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
	{
		xemacif_input(&server_netif);
	}
	if (dhcp_timoutcntr <= 0)
	{
		if ((server_netif.ip_addr.addr) == 0)
		{
			xil_printf("DHCP Timeout\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(server_netif.ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255,  0);
			IP4_ADDR(&(server_netif.gw),      192, 168,   1,  1);
		}
	}

	ipaddr.addr = server_netif.ip_addr.addr;
	gw.addr = server_netif.gw.addr;
	netmask.addr = server_netif.netmask.addr;
	configure_callbacks();

	while (1)
	{
		if (SHAREDMEM[APP_KEY] == 2)
		{
			break;
		}
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(&server_netif);
		if (sending == 1)
		{
			send_data();
		}
	}
	return 0;
}
