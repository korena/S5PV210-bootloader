.equ GPIO_BASE,  0xE0200000
.equ GPJ2CON_OFFSET,  0x280   
.equ GPJ2DAT_OFFSET,   0x284
.equ GPJ2PUD_OFFSET,   0x288
.equ GPJ2DRV_SR_OFFSET,  0x28C
.equ GPJ2CONPDN_OFFSET,  0x290
.equ GPJ2PUDPDN_OFFSET,  0x294


.text
.code 32

.global _BL2
.global copy_lim


.section .rodata
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

 _end:  
	mov r0,#0xC
        bl      flash_led

	mov r1,#0x100000
1:	subs r1,r1,#1
	bne 1b
	
	mov r0,#0xF
        bl      flash_led

	mov r1,#0x100000
2:	subs r1,r1,#1
	bne 2b
        b       _end



flash_led:
     ldr r4,=(GPIO_BASE+GPJ2CON_OFFSET)
     ldr r5,[r4]
     ldr r2,=1
     orr r5,r5,r2
     orr r5,r2,lsl#4
     orr r5,r2,lsl#8
     orr r5,r2,lsl#12
     orr r5,r5,r2
     str r5,[r4]
     ldr r4,=(GPIO_BASE+GPJ2DAT_OFFSET)
     ldr r5,[r4]
     ldr r3,=0xF
     orr r5,r5,r3  @ turn them all off ...
     bic r5,r5,r0
     str r5,[r4]
     mov r1, #0x10000  @ this should be passed meh!
1:  subs r1, r1, #1
  bne 1b
     orr r5,r5,r3  @ turn them all off again ...
     str r5,[r4]
     mov pc, lr
	
exec_sdram_string:
.ascii "test code execution from dram successful! ...\n"
.set exec_sdram_len,.-exec_sdram_string
.align 4
.set copy_lim,.-_BL2
