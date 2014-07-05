.text
.code 32
.global _bl2_entry

_bl2_entry:
	ldr	r0,=exec_sdram_string
	ldr	r1,=exec_sdram_len
	bl	uart_print_string
	b	.


uart_print_string:  @ yup, another definition, cause I can.
        stmfd sp!,{r2-r4,lr}
        ldr     r2, =0xE2900000
1:
        ldrb    r3,[r0],#1
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b
        strb    r3,[r2,#0x20]    
        subs    r1,r1,#1
        bne     1b
        ldmfd sp!,{r2-r4, pc}


.section rodata

jump_sdram_string:
.ascii "attempting code execution from dram ...\r\n"
.set jump_sdram_len,.-jump_sdram_string
exec_sdram_string:
.ascii "code execution from dram successful! ...\r\n"
.set exec_sdram_len,.-exec_sdram_string


