.text
.code 32
.global _bl2_entry

.equ ram_load_address,          0x20000000

_bl2_entry:
	ldr	r0,=starting_bl2_string 
	bl	uart_print

        bl      clock_subsys_init

        bl 	mem_ctrl_asm_init


@ wierd problem testing ...
@ the processor doesn't seem to be capable of
@ accessing the following range of addresses,
@ this could be an execution mode thingy (some sort of security feature), or
@ some hardware issue (unlikely)
	ldr r1,=0xEB000006
	ldr r0,[r1]
	bl uart_print_hex 
	ldr r1,=0xEB00000C
	ldr r0,[r1]
	bl uart_print_hex 
	ldr r1,=0xEB00000E
	ldr r0,[r1]
	bl uart_print_hex 


	b    .
@	b 	start_linux 

        /* Memory test: copy a block of code from read only memory to ram,
         * and jump to execute it, the executed code should give a
         * certain message if successful*/

@        bl      copy_To_Mem
@        ldr     r0,=0x0C  @ corrupt r0
@        ldr     ip,=ram_load_address
@        mov     lr, pc
@        bx      ip


.section .rodata
starting_bl2_string:
.ascii "bl2 executing ...\n\r\0"
.align 4
