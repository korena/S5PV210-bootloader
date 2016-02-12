#include <stdint.h>

#define SDMASYSAD0		((uint32_t)0xEB000000)

#define BLKSIZE0		((uint32_t)0xEB000004)
#define BLKSIZE0_BITS_MASK   	((uint32_t)0x0FFF)
#define BUFBOUND_BITS_MASK	((uint32_t)0x7000)

#define BLKCNT0			((uint32_t)0xEB000006)
#define ARGUMENT0		((uint32_t)0xEB000008)

#define TRNMOD0			((uint32_t)0xEB00000C)
#define ENDMA_BIT_MASK		((uint32_t)0x00000001)
#define ENBLKCNT_BIT_MASK	((uint32_t)0x00000002)
#define ENACMD12_BIT_MASK	((uint32_t)0x00000004)
#define RD1WT0_BIT_MASK		((uint32_t)0x00000010)
#define MUL1SIN0_BIT_MASK	((uint32_t)0x00000020)
#define CCSCON_BITS_MASK	((uint32_t)0x00000300)
#define BOOTCMD_BIT_MASK	((uint32_t)0x00001000)
#define BOOTACK_BIT_MASK	((uint32_t)0x00002000)

#define CMDREG0			((uint32_t)0xEB00000E)
#define RSPTYP			((uint32_t)0x00000003)
#define ENCMDCRC		((uint32_t)0x00000008)
#define ENCMDIDX		((uint32_t)0x00000010)
#define DATAPRNT		((uint32_t)0x00000020)
#define CMDTYP			((uint32_t)0x000000C0)
#define CMDIDX			((uint32_t)0x00003F00)

#define COPY_BL2_SIZE	(80*1024)   // 80KB (binary Kilo)
extern void uart_print_string(char*str,uint32_t len);
extern void uart_print_hex(uint32_t address);
extern void uart_print(char*str);


typedef uint32_t (*copy_mmc_to_mem)(uint32_t  channel, uint32_t  start_block, uint16_t block_size,
		                                            uint32_t  *target, uint32_t  init);
 


/*Lazy macros*/
#define READ_REG(reg)		(*(volatile uint32_t*)(reg))
#define WRITE_REG(reg,val) 	((*(volatile uint32_t*)(reg))=(val))

void debug_print(char*str){
	uart_print(str);
}

void copy_bl2_to_sram(void){
	uint32_t *load_address =(uint32_t*) (0xD0020FB0);  // SRAM BL1 start address + 16k (binary kilo)
	debug_print("Copying BL2 started ...\n");
	void (*BL2)(void);
	uint32_t channel = 0;
	copy_mmc_to_mem copy_func = (copy_mmc_to_mem) (*(uint32_t *) 0xD0037F98); //SdMccCopyToMem function from iROM documentation
	uint32_t ret = copy_func(channel, 33, COPY_BL2_SIZE/512,load_address, 0);
	if(ret == 1){
		debug_print("BL2 loading successful, running it ...\n");
		BL2 = (void*) load_address;
		(*BL2)(); // dereferencing and running ...
	}else{
		debug_print("BL2 loading failed :-(\n");
	}
}

