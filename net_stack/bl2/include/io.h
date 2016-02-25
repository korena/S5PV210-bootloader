#ifndef _IO_H_
#define _IO_H_

/*
 * This is not strictly necessary, but its nice to have.
 * These functions are defined in asm/io.s
 */

.text

.global __raw_writeb
.global __raw_writew
.global __raw_writel
.global __raw_readb
.global __raw_readw
.global __raw_readl


#endif
