.text
.code 32
.global _bl2_entry
.global doNotOptimize
.equ ram_load_address,          0x20000000

_bl2_entry:
	ldr	r0,=starting_bl2_string 
	bl	uart_print

        bl      clock_subsys_init

        bl 	mem_ctrl_asm_init

	b 	start_linux 

        /* Memory test: copy a block of code from read only memory to ram,
         * and jump to execute it, the executed code should give a
         * certain message if successful*/

@        bl      copy_To_Mem
@        ldr     r0,=0x0C  @ corrupt r0
@        ldr     ip,=ram_load_address
@        mov     lr, pc
@        bx      ip

/* Empty void doNotOptimize(void) label, for GCC. */
doNotOptimize:
        stmfd sp!,{lr}
        ldmia sp!,{pc}

.section .rodata
starting_bl2_string:
.ascii "bl2 executing ...\n\r\0"
.align 4
