#ifndef NETWORK
#define NETWORK

#include <stdio.h>
#include <string.h>

#include "data.h"
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xil_cache.h"

#include "lwip/dhcp.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "xil_printf.h"

#define MAC_ADDRESS {0x00, 0x0a, 0x35, 0x00, 0x01, 0x02}
#define PORT 5001

void init_network(int core_select, volatile uint32_t *shared_memory);
int start_network();

#endif
