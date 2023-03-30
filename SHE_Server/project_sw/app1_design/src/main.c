#include <stdio.h>
#include <inttypes.h>
#include "platform.h"
#include "xpseudo_asm.h"
#include "xil_printf.h"
#include "sleep.h"

#include "app.h"
#include "configuration.h"
#include "code.h"
#include "data.h"
#include "hardware.h"
#include "homenc.h"

extern volatile uint32_t * core_config;

volatile uint32_t * SHAREDMEM;

POLYNOMIAL    *	poly;
IN_CIPHERTEXT * in_ct ;
OUT_CIPHERTEXT* out_ct;
RLK_CONSTANTS * rlkconstants;

void init_SharedMemory(void)
{
	SHAREDMEM = (uint32_t *) 0xFFFC0000;

	uint64_t base;
	base   = ((uint64_t)SHAREDMEM[4]) + ((uint64_t)SHAREDMEM[5] << 32);
	poly   = (POLYNOMIAL*) base;

#if APP == 0
	base   = ((uint64_t)SHAREDMEM[6]) + ((uint64_t)SHAREDMEM[7] << 32);
	in_ct  = (IN_CIPHERTEXT*) base;

	base   = ((uint64_t)SHAREDMEM[8]) + ((uint64_t)SHAREDMEM[9] << 32);
	out_ct = (OUT_CIPHERTEXT*) base;
#else
	base   = ((uint64_t)SHAREDMEM[10]) + ((uint64_t)SHAREDMEM[11] << 32);
	in_ct  = (IN_CIPHERTEXT*) base;

	base   = ((uint64_t)SHAREDMEM[12]) + ((uint64_t)SHAREDMEM[13] << 32);
	out_ct = (OUT_CIPHERTEXT*) base;
#endif

	base   = ((uint64_t)SHAREDMEM[14]) + ((uint64_t)SHAREDMEM[15] << 32);
	rlkconstants = (RLK_CONSTANTS*) base;
}

int main()
{
    init_platform();
    sleep(2);
    init_hardware();
    init_SharedMemory();
	sleep(2);

    printf("Application Core %d is ready\n\r", APP);
    while (SHAREDMEM[APP_KEY] != 2)
    {
    	if (SHAREDMEM[APP] == 1)
    	{
    		int key = SHAREDMEM[APP_KEY];
    		if (key == 1)
    		{
    			Write_Inputs_to_FPGA    (in_ct);
    			ExecuteCode             ();
    			Read_Outputs_from_FPGA  (out_ct);
    			SHAREDMEM[APP_KEY] = 0;
    		}
    	}
    }
    return 0;
}
