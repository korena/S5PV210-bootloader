.text
.code 32

.global _BL2
.global copy_lim


.section .rodata
.align 4,0x90
_BL2:
        ldr     r0,=exec_sdram_string
        ldr     r1,=exec_sdram_len
        ldr     r2, =0xE2900000
1:
        ldrb    r3,[r0],#1
        mov     r4, #0x10000 @ delay
2:      subs    r4, r4, #1
        bne     2b
        strb    r3,[r2,#0x20]   @ This is the UTXH register, the transmit buffer. 
        subs    r1,r1,#1
        bne     1b
	
        ldr     r0,=0xD   @ This is to get a unique LED pattern 
        mov     pc,lr
	
exec_sdram_string:
.ascii "test code execution from dram successful! ...\r\n"
.set exec_sdram_len,.-exec_sdram_string
.align 4
.set copy_lim,.-_BL2
