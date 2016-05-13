#include <stdint.h>


#define COPY_BL2_SIZE	(80*1024)   // 80KB (binary Kilo)
extern void uart_print_string(char*str,uint32_t len);
extern void uart_print_hex(uint32_t address);
extern void uart_print(char*str);


typedef uint32_t (*copy_mmc_to_mem)(uint32_t  channel, uint32_t  start_block, uint16_t block_size,
		                                            uint32_t  *target, uint32_t  init);
void debug_print(char*str){
	uart_print(str);
}

void copy_bl2_to_sram(void){
	uint32_t *load_address =(uint32_t*) (0xD0020FB0);  // SRAM BL1 start address + 16k (binary kilo)
	debug_print("Copying BL2 started ...\n\r");
	void (*BL2)(void);
	uint32_t channel = 0;
	copy_mmc_to_mem copy_func = (copy_mmc_to_mem) (*(uint32_t *) 0xD0037F98); //SdMccCopyToMem function from iROM documentation
	uint32_t ret = copy_func(channel, 33, COPY_BL2_SIZE/512,load_address, 0);
	if(ret == 1){
		debug_print("BL2 loading successful, running it ...\n\r");
		debug_print("The address where BL2 should be contains:\n\r");
		uart_print_hex(*load_address);
		BL2 = (void*) load_address;
		(*BL2)(); // dereferencing and running ...
	}else{
		debug_print("BL2 loading failed :-(\n\r");
	}
}

