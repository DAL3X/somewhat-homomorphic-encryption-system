#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "../homoenc/hardware/data.hpp"
#include "network_parameter.hpp"

// Socket number and addresses
int sockfd, connfd;
struct sockaddr_in servaddr, cli;

/*
 * Recieves a result ciphertext from the server.
*/
int rec_ciphertext(OUT_CIPHERTEXT *ct)
{
	char *ct_pointer = (char*)ct->c0[0];
	while (ct_pointer != (char*)ct + sizeof(OUT_CIPHERTEXT))
	{
		size_t size = read(sockfd, ct_pointer, MAX_PACKET_SIZE);
		if (size > MAX_PACKET_SIZE || size < 1)
		{
		printf("Failed to recieve part of a ciphertext! %d \n", size);
		return -1;
		}
		ct_pointer = ct_pointer + size;
	}
	return 0;
}

/*
 * Sends ciphertexts to the server for calculation.
*/
int send_ciphertext(IN_CIPHERTEXT *ct)
{
	size_t size = write(sockfd, ct, sizeof(*ct));
	if (size != sizeof(*ct))
	{
		printf("Failed to send ciphertexts!\n");
		return -1;
	}
    return 0;
}

/*
 * Sends relinarisation keys to the server for later use.
*/
int send_relin(RLK_CONSTANTS *relin)
{
	size_t size = write(sockfd, relin, sizeof(*relin));
	if (size != sizeof(*relin))
	{
		printf("Failed to send relinearisation keys!\n");
		return -1;
	}
    return 0;
}

/*
 * Closed the connection to the server.
*/
int close_connection()
{
	close(sockfd);	
    return 0;
}

/*
 * Opens a connection to the server.
*/
int open_connection()
{
    sockfd = socket(AF_INET, SOCK_STREAM, SERVER_TYPE);
    if (sockfd == -1) 
	{
        printf("socket creation failed...\n");
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(PORT);
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
	{
        printf("connection with the server failed...\n");
        return(-1);
    }
    printf("Connected to the server\n");
    return 0;
}
