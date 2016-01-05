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


.text
.code 32

.global uart_asm_init
.global uart_print_hex
.global uart_print_string


uart_asm_init:
        stmfd sp!,{lr}
        /* set GPIO(GPA) to enable UART */
        @ GPIO setting for UART
        ldr     r0, =GPIO_BASE  @0xE0200000
        ldr     r1, =0x22222222
        str     r1, [r0,#GPA0CON_OFFSET]  @ storing 0010 in all reg fields, which configures PA0 for UART 0 and UART 1.

        ldr     r0, =UART_CONSOLE_BASE

        mov     r1, #0x0
        str     r1, [r0,#UFCON_OFFSET] @ resetting all bits in UFCON0 register, disabling FIFO, which means the Tx/Rx buffers 
                                       @ offer a single byte to hold transfer/receive data
        str     r1, [r0,#UMCON_OFFSET] @ resetting all bits in UMCON0 register, disabling auto flow control (AFC), and setting 'Request to 
                                            @ Send' bit to zero, which basically means that our UART0 module will tell the other side of the
                                            @ communication line that we can never receive anything, however, it is up to the sender to respect
                                            @ this, depending on it's configuration.

        mov     r1, #0x3
        str     r1, [r0,#ULCON_OFFSET]     @ setting bits [0:1] to 0b11, which means data packets are 8 bits long, standard stuff.

        ldr     r1, =0x3c5                  @ (0011-1100-0101)
        str     r1, [r0,#UCON_OFFSET]       @ Receive Mode: interrupt/polling mode, Transmit Mode: interrupt/polling mode,
                                            @ Send Break Signal: No, Loop-back Mode: disabled, Rx Error Status Interrupt: Enable,
                                            @ Rx Time Out: Enable,Rx Interrupt Type: level, Tx Interrupt Type: level.
                                            @  Clock Selection: PCLK,dont care for the rest.

        ldr     r1, =UART_UBRDIV_VAL   @ 34
        str     r1, [r0,#UBRDIV_OFFSET]

        ldr     r1, =UART_UDIVSLOT_VAL  @ 0xDDDD
        str     r1, [r0,#UDIVSLOT_OFFSET]
        
	ldr     r0,=init_uart_string
        mov     r1,#init_uart_len
        bl      uart_print_string

        ldmfd sp!,{pc}


/*void uart_print_string(char* string, int size)*/

.align 4,0x90
uart_print_string:
        stmfd sp!,{r2-r4,lr}
        ldr     r2, =UART_CONSOLE_BASE          @0xE29000000
1:
        ldrb    r3,[r0],#1
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b
        strb    r3,[r2,#UTXH_OFFSET]    
        subs    r1,r1,#1
        bne     1b
        ldmfd sp!,{r2-r4, pc}

/*void uart_print_hex(uint32_t hexToPrint)*/
.align 4,0x90
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
        ldmfd sp!,{r0-r4, pc}


.section .rodata
init_uart_string:
.ascii "UART 0 Initialization complete ...\r\n"
.set init_uart_len,.-init_uart_string
