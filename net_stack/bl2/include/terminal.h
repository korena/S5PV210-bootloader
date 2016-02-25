#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <stdint.h>
/************** function prototypes ******************/



/*
 * Not strictly required, but gives me an idea of where things are ...
 * These are globalized in asm/uart_mod.s
 */

extern void uart_print(char* str);
extern void uart_print_hex(uint32_t integer);


void printf(const char* fmt, ...);
#endif
