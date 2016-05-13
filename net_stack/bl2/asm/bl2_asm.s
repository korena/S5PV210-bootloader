.text
.code 32
.global start
.global doNotOptimize
.equ sromc_bw,			0xE8000000
.equ ram_load_address,          0x20000000
.equ sromc_bw_bank_1_setting,   0x000000d0
start:
	ldr	r0,=starting_bl2_string 
	bl	uart_print

        bl      clock_subsys_init

@ setting srom bank1 address width and some other properties
	ldr 	r0,=sromc_bw
	ldr 	r0,[r0]
	orr 	r1,r0,#sromc_bw_bank_1_setting
	ldr 	r0,=sromc_bw
	str 	r1,[r0]

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
