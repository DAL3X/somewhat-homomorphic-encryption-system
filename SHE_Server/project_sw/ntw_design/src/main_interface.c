#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sleep.h>

#include "platform.h"
#include "xil_printf.h"
#include "xil_cache.h"

#include "configuration.h"
#include "performance.h"
#include "data.h"
#include "network.h"


#define NUM_PRIME 6
long long int p[] = {1068564481,1069219841,1070727169,1071513601,1072496641,1073479681,1068433409,
					 1068236801,1065811969,1065484289,1064697857,1063452673,1063321601};

volatile uint32_t * SHAREDMEM;

POLYNOMIAL poly; // Not used in this version
IN_CIPHERTEXT in_cipher0;
IN_CIPHERTEXT in_cipher1;
OUT_CIPHERTEXT out_cipher0;
OUT_CIPHERTEXT out_cipher1;
RLK_CONSTANTS rlkeys;

void init_SharedMemory(void)
{
	SHAREDMEM 		  = (uint32_t *) 0xFFFC0000;



    SHAREDMEM[0]  = 0;
    SHAREDMEM[1]  = 0;
    SHAREDMEM[2]  = 0;
    SHAREDMEM[3]  = 0;

    SHAREDMEM[4]  = (uint32_t) ((uint64_t)poly.p0[0]		& 0xFFFFFFFF);
    SHAREDMEM[5]  = (uint32_t) ((uint64_t)poly.p0[0]		>> 32       );

    SHAREDMEM[6]  = (uint32_t) ((uint64_t)in_cipher0.c00[0] & 0xFFFFFFFF);
	SHAREDMEM[7]  = (uint32_t) ((uint64_t)in_cipher0.c00[0] >> 32       );

	SHAREDMEM[8]  = (uint32_t) ((uint64_t)out_cipher0.c0[0]	& 0xFFFFFFFF);
	SHAREDMEM[9]  = (uint32_t) ((uint64_t)out_cipher0.c0[0]	>> 32       );

    SHAREDMEM[10] = (uint32_t) ((uint64_t)in_cipher1.c00[0]	& 0xFFFFFFFF);
	SHAREDMEM[11] = (uint32_t) ((uint64_t)in_cipher1.c00[0]	>> 32       );

	SHAREDMEM[12] = (uint32_t) ((uint64_t)out_cipher1.c0[0]	& 0xFFFFFFFF);
	SHAREDMEM[13] = (uint32_t) ((uint64_t)out_cipher1.c0[0]	>> 32       );

	SHAREDMEM[14] = (uint32_t) ((uint64_t)rlkeys.rlk00[0] 	& 0xFFFFFFFF);
	SHAREDMEM[15] = (uint32_t) ((uint64_t)rlkeys.rlk00[0] 	>> 32       );
}

int main()
{
    init_platform();
	Xil_DCacheFlush();
	Xil_DCacheDisable();

	init_SharedMemory();
    sleep(1);
    init_network(0, SHAREDMEM);
    printf("Interfacing Core is ready\n\r");

	start_network(); // network contains the main loop

    cleanup_platform();
    return 0;
}
