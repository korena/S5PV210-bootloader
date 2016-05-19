#ifndef __CONFIGS_H_
#define __CONFIGS_H_

#include<stdint.h>



#define DRAM_BASE 0x20000000
#define ZIMAGE_LOAD_ADDRESS (uint32_t*) (DRAM_BASE + 0x8000)  // 32k away from the base address of DRAM



/* network stack choices*/
#define NET_RETRY 1
#define NET_RETRY_FOREVER 1

uint32_t load_addr;		/* Default Load Address */
unsigned long save_addr;		/* Default Save Address */
unsigned long save_size;		/* Default Save Size */


#endif /* __CONFIGS_H_*/
