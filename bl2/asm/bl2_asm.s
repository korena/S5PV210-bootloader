.text
.code 32
.global _bl2_entry

.equ ram_load_address,          0x20000000

_bl2_entry:


        bl clock_subsys_init

        bl mem_ctrl_asm_init

        /* Memory test: copy a block of code from read only memory to ram,
         * and jump to execute it, the executed code should give a
         * certain message if successful*/

        bl      copy_To_Mem
        ldr     r0,=0x0C  @ corrupt r0
        ldr     ip,=ram_load_address
        mov     lr, pc
        bx      ip

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



