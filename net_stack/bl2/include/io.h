#ifndef _IO_H_
#define _IO_H_

#include <string.h>
/*
 * This is not strictly necessary, but its nice to have.
 * These functions are defined in asm/io.s
 */

extern void _raw_writeb(uint8_t d,uint32_t register);
extern void _raw_writehw(uint16_t d,uint32_t register);
extern void _raw_writel(uint32_t d,uint32_t register);
extern uint8_t  _raw_readb(uint32_t register);
extern uint16_t _raw_readhw(uint32_t register);
extern uint32_t _raw_readl(uint32_t register);



/*ualigned address capable memcpy*/

extern void *ul_memcpy(void *dest, const void *src, size_t n);

#endif
