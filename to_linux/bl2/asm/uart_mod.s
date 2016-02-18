/*For UART*/
.equ GPA0CON_OFFSET,            0x000
.equ GPA1CON_OFFSET,            0x020
.equ UART_BASE,                 0XE2900000
.equ UART0_OFFSET,              0x0000

.equ ULCON_OFFSET,                  0x00
.equ UCON_OFFSET,                   0x04
.equ UFCON_OFFSET,                  0x08
.equ UMCON_OFFSET,                  0x0C
.equ UTXH_OFFSET,                   0x20
.equ UBRDIV_OFFSET,                 0x28
.equ UDIVSLOT_OFFSET,       0x2C
.equ UART_UBRDIV_VAL,       34  
.equ UART_UDIVSLOT_VAL,         0xDDDD

.equ GPIO_BASE,  0xE0200000
.equ UART_CONSOLE_BASE, (UART_BASE + UART0_OFFSET)



.equ TERM, 0x00

.text
.code 32

.global uart_print
.global uart_print_hex
.global uart_print_string


/*void uart_print_string(char* string, int size)*/

uart_print_string:
        stmfd sp!,{r0-r4,lr}
        ldr     r2, =UART_CONSOLE_BASE          @0xE29000000
1:
        ldrb    r3,[r0],#1
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b
        strb    r3,[r2,#UTXH_OFFSET]    
        subs    r1,r1,#1
        bne     1b
        ldmia sp!,{r0-r4, pc}

/*void uart_print_hex(uint32_t hexToPrint)*/
uart_print_hex:
        stmfd sp!,{r0-r4,lr}
	mov r1,r0
	mov r2,#8
loopLag:
	mov	r1,r1,ror #28
	and	r0,r1,#0x0000000F  @ mask
	
	cmp r0,#10
	bge hexVal
	add r0,r0,#0x30
	bal printIt
hexVal:
	add r0,r0,#0x37
printIt:
	ldr	r3,=UART_CONSOLE_BASE 
	strb    r0,[r3,#0x20]   @ This is the UTXH register, the transmit buffer.	
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b

	sub	r2,r2,#1
	cmp	r2,#0
	bne	loopLag	
	mov	r0,#0x0D
        strb    r0,[r3,#0x20]
        mov     r4, #0x10000 @ delay
3:      subs    r4, r4, #1
	bne     3b
        mov     r0,#0x0A
	strb	r0,[r3,#0x20]
#	mov	pc,lr
        ldmfd sp!,{r0-r4, pc}

uart_print:
	stmfd sp!,{r0-r4, lr}
        ldr     r2, =UART_CONSOLE_BASE          @0xE29000000
1:
        ldrb    r3,[r0],#1
	cmp	r3,#TERM
	beq	done
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b
        strb    r3,[r2,#UTXH_OFFSET]    
        b       1b
done:
        ldmfd sp!,{r0-r4, pc}

.section .rodata
init_uart_string:
.ascii "UART 0 Initialization complete ...\n\r\0"
.set init_uart_len,.-init_uart_string
