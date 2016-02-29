#ifndef _IO_H_
#define _IO_H_

/*
 * This is not strictly necessary, but its nice to have.
 * These functions are defined in asm/io.s
 */

void __raw_writeb(uint8_t d,uint32_t register);
void __raw_writew(uint16_t d,uint32_t register);
void __raw_writel(uint32_t d,uint32_t register);
uint8_t  __raw_readb(uint32_t register);
uint16_t __raw_readw(uint32_t register);
uint32_t __raw_readl(uint32_t register);


#endif
