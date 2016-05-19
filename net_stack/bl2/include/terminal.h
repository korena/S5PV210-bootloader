#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <stdint.h>
#include <stdio.h>
/************** function prototypes ******************/

/*
 * Not strictly required, but gives me an idea of where things are ...
 * These are globalized in asm/uart_mod.s
 */

extern void uart_print(const char* str);
extern void uart_print_hex(uint32_t integer);
extern void uart_print_string(const char* str,uint32_t len);



int print_format(const char* fmt, ...); // MISRA C hates this :-D
//char *strcpy(char *strDest, const char *strSrc);
int printnum (char *__restrict__ s, size_t maxlen, const char *__restrict__ format, uint32_t num);
#endif
