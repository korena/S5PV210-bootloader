/*
 * SDRAM Controller
 */
.equ APB_DMC_0_BASE,			0xF0000000
.equ ASYNC_MSYS_DMC0_BASE,		0xF1E00000

.equ DMC_CONCONTROL, 			0x00
.equ DMC_MEMCONTROL, 			0x04
.equ DMC_MEMCONFIG0, 			0x08
.equ DMC_MEMCONFIG1, 			0x0C
.equ DMC_DIRECTCMD, 			0x10
.equ DMC_PRECHCONFIG, 		0x14
.equ DMC_PHYCONTROL0, 		0x18
.equ DMC_PHYCONTROL1, 		0x1C
.equ DMC_RESERVED, 			0x20
.equ DMC_PWRDNCONFIG, 		0x28
.equ DMC_TIMINGAREF, 			0x30
.equ DMC_TIMINGROW, 			0x34
.equ DMC_TIMINGDATA, 			0x38
.equ DMC_TIMINGPOWER, 		0x3C
.equ DMC_PHYSTATUS, 			0x40
.equ DMC_CHIP0STATUS, 		0x48
.equ DMC_CHIP1STATUS, 		0x4C
.equ DMC_AREFSTATUS, 			0x50
.equ DMC_MRSTATUS, 			0x54
.equ DMC_PHYTEST0, 			0x58
.equ DMC_PHYTEST1, 			0x5C

.equ MP1_0DRV_SR_OFFSET, 		0x3CC
.equ MP1_1DRV_SR_OFFSET, 		0x3EC
.equ MP1_2DRV_SR_OFFSET, 		0x40C
.equ MP1_3DRV_SR_OFFSET, 		0x42C
.equ MP1_4DRV_SR_OFFSET, 		0x44C
.equ MP1_5DRV_SR_OFFSET, 		0x46C
.equ MP1_6DRV_SR_OFFSET, 		0x48C
.equ MP1_7DRV_SR_OFFSET, 		0x4AC
.equ MP1_8DRV_SR_OFFSET, 		0x4CC

.equ ram_load_address,          0x20000000

.equ GPIO_BASE,  0xE0200000

.text
.code 32
.global copy_To_Mem
.global mem_ctrl_asm_init
.global copy_far_To_Mem


mem_ctrl_asm_init:
	stmfd sp!,{lr}
	ldr	r0, =ASYNC_MSYS_DMC0_BASE
	ldr	r1, =0x0
	str	r1, [r0, #0x0]

	ldr	r1, =0x0
	str	r1, [r0, #0xC]
        
	/* DMC0 Drive Strength (Setting 4X) */
	ldr	r0, =GPIO_BASE

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_0DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_1DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_2DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_3DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_4DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_5DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_6DRV_SR_OFFSET]

	ldr	r1, =0x0000FFFF
	str	r1, [r0, #MP1_7DRV_SR_OFFSET]

	/*
	MP1_8[0]   Xm1CSn[0]   [1:0] => [1:1]
	MP1_8[1]   Xm1CSn[1]   [3:2] => [1:1]
	MP1_8[2]   Xm1RASn     [5:4] => [1:1]
	MP1_8[3]   Xm1CASn     [7:6] => [1:1]
	MP1_8[4]   Xm1WEn      [9:8] => [1:1]
	MP1_8[5]   Xm1GateIn   [11:10] => [1:1]
	MP1_8[6]   Xm1GateOut  [13:12] => [1:1]
	*/
	
	ldr	r1, =0x00003FFF
	str	r1, [r0, #MP1_8DRV_SR_OFFSET]
	
	/* DMC0 initialization*/
	ldr	r0, =APB_DMC_0_BASE
	
	ldr	r1, =0x00101000				@PhyControl0 DLL parameter setting,
	str	r1, [r0, #DMC_PHYCONTROL0]
	
	ldr	r1, =0x00000086				@PhyControl1 DLL parameter setting
	str	r1, [r0, #DMC_PHYCONTROL1]		@ (0000-0000-0000-0000-0000-0000-1000-0-110)

	ldr	r1, =0x00101002				@PhyControl0 DLL on
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00101003				@PhyControl0 DLL start
	str	r1, [r0, #DMC_PHYCONTROL0]

find_lock_val:
	ldr	r1, [r0, #DMC_PHYSTATUS]		@Load Phystatus register value
	and	r2, r1, #0x7				@masking bits [0:2] of PHYSTATUS register.
	cmp	r2, #0x7				
	bne	find_lock_val				@ Loop until DLL is locked, the loop waits for ctrl_clock, ctrl_flock and ctrl_locked 
							@ to be set

	
  /*block of code if we plan to not use DLL--start*/
.if 0	
	and	r1, #0x3fc0				@  masking ctrl_lock_value[9:2] number of delay cells for coarse lock (mask = 11111111000000, bits 						@  [0:5] cleared, bits [6:13] set
	mov	r2, r1, LSL #18				@ r2 = VVVVVVVV000000000000000000000000, bits [0:23] cleared, bits [24:31] value bits
	orr	r2, r2, #0x100000			@ r2 = VVVVVVVV000000000000000000100000 
	orr	r2 ,r2, #0x1000				@ r2 = VVVVVVVV000000000000000000101000 

	orr	r1, r2, #0x3				@ Force Value locking (r1 = VVVVVVVV000000000000000000101011)
	str	r1, [r0, #DMC_PHYCONTROL0]		@ Store r1 in PHYCONTROL0, changing only the upper 8 bits [24:31], as explained in the
 							@ documentation, This field is used instead of ctrl_lock_value[9:2] from the
           						@ PHY DLL when ctrl_dll_on is LOW. (i.e. If the DLL is off, this field is used to
                                                        @ generate  270' clock and  shift DQS by 90'.)

	orr	r1, r2, #0x1				@DLL off
	str	r1, [r0, #DMC_PHYCONTROL0]
.endif
 /*block of code if we plan to not use DLL--end */

	ldr	r1, =0x0FFF2010				@ConControl auto refresh off (1111111111110010000000010000, bits [16:27],[13] and [4] 
	str	r1, [r0, #DMC_CONCONTROL]		@ set).
							@ Following figure 1-7, we set the rd_fetch time to 2, accounting for the innevitable
                                                        @ existence of tDQSCK (skew between DQS and Ck), the rest of the bits are left
							@ unchanged.
							@ this maps to point 5.


	ldr	r1, =0x00202400				@(was DMC0_MEMCONTROL) MemControl BL=4, 1 chip, DDR2 type, dynamic self refresh off,
	str	r1, [r0, #DMC_MEMCONTROL]			@ power down modes off.
								@  0x00202400 (0000-0000-0010-0000-0010-0100-0000-0000)


	ldr	r1, =0x20F00322				@ MemConfig0 512MB config, 4 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 
	str	r1, [r0, #DMC_MEMCONFIG0]		@ 2:Mixed 
							@ 0x20F01422 (0010-0000-1111-0000-0000-0011-0010-0010) 0x20E01323
					                @ 4 banks [3:0] = 0x2, row address bits = 14 => [4:7] = 0x2 (found in K4T1G084QF 5. 
	                                                @ DDR2 SDRAM Addressing),col address bits = 11 => [11:8] = 0x4 (I think!), memory  
	                                                @ address scheme  explained in 5.3 Address Mapping Scheme [15:12] = 0x0.
	                                                @ [23:16] kept at default, [31:24] also kept as default. 
							@ NOTE (Feb 23rd,2016)there was a mistake in the number of column address bits that
							@ caused the memory overwrite effect faced when copying from sdmmc to sdram.

	ldr	r1, =0xFF000000				@ PrechConfig    this maps to point 8 in the sequence ... these are choices you
        str	r1, [r0, #DMC_PRECHCONFIG]              @  can finetune later
	                                                @ left as defaults for now.

	ldr	r1, =0x50F			@ Following the formula in the TIMINGAREF table, we will place 7.8 us * 166 MHz =  0x50F
 	str	r1, [r0, #DMC_TIMINGAREF]		@ in bits [0:15]
	                                                
	ldr	r1, =0x18233287			@ TimingRow	for @166Mhz  tRfc=105ns tRRD=10ns tRP=15ns trcd = 15ns trc = 60ns tRAS=40ns
	str	r1, [r0, #DMC_TIMINGROW]        @ 0x18233287  (0001-1000-0010-0011-0011-001010-000111)  

	ldr	r1, =0x23230000			@ TimingData	CL=3
	str	r1, [r0, #DMC_TIMINGDATA]       @   tWTR=2ns tWR=15ns tRTP= CL=3

	ldr	r1, =0x07150232			@ TimingPower
	str	r1, [r0, #DMC_TIMINGPOWER]      @ 0x07150232  (00-000111-00010101-00000010-0011-0010)
 
	ldr	r1, =0x07000000				@DirectCmd	chip0 NOP
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01000000				@DirectCmd	chip0 PALL
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00020000				@DirectCmd	chip0 EMRS2
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00030000				@DirectCmd	chip0 EMRS3
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00010400				@DirectCmd	chip0 EMRS1 (MEM DLL on, DQS# disable)
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00000552				@DirectCmd	chip0 MRS (MEM DLL reset) CL=3, BL=4
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01000000				@DirectCmd	chip0 PALL
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05000000				@DirectCmd	chip0 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05000000				@DirectCmd	chip0 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00000452				@DirectCmd	chip0 MRS (MEM DLL unreset)
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00010780				@DirectCmd	chip0 EMRS1 (OCD default)
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00010400				@DirectCmd	chip0 EMRS1 (OCD exit)
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x0FF02030				@ConControl	auto refresh on
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0xFFFF00FF				@PwrdnConfig
	str	r1, [r0, #DMC_PWRDNCONFIG]

	ldr	r1, =0x00202400				@MemControl	BL=4, 1 chip, DDR2 type, dynamic self refresh, force precharge, 
	str	r1, [r0, #DMC_MEMCONTROL]		@ dynamic power down off

	ldr r0,=init_sdram_string

@	mov r1,#init_sdram_len

	bl  uart_print


        ldmfd sp!,{pc}


copy_To_Mem:
        stmfd sp!,{r4-r11,lr}

        ldr     r0,=copy_sdram_start_string
        ldr     r1,=copy_sdram_start_len
        bl      uart_print_string
        ldr     r0,=_TESTOS
        ldr     r1,=ram_load_address
        ldr     r2,=copy_lim
1:      ldr     r3,[r0],#4
        str     r3,[r1],#4
        subs    r2,r2,#1
        bne     1b
	/*test the first and last words of the copied segment, if they match, assume successful*/
	
	ldr	r3,=_TESTOS
	ldr	r0,[r3]
	ldr     r1,=ram_load_address
	ldr	r5,=copy_lim
	ldr	r2,[r1]  @ the contents of the ram load address.
	cmp	r0,r2 @ This is test one, if not equal, go to exit_copy.
	bne	exit_copy
	ldr	r0,[r3,r5]	
	ldr	r4,[r1,r5]
	cmp	r0,r4 @ This is test two, if not equal, go to exit_copy.
        ldr     r0,=copy_sdram_end_string
        ldr     r1,=copy_sdram_end_len
        bl      uart_print_string
	ldmfd sp!,{r4-r11,pc}
exit_copy:
        ldr   r0,=copy_sdram_err_string
        ldr   r1,=copy_sdram_err_len
        bl    uart_print_string
	ldr   r1,=_TESTOS
	ldr   r0,[r1]
	bl    uart_print_hex
	ldr   r1,=ram_load_address
	ldr   r0,[r1]
	bl    uart_print_hex

        ldr   r1,=_TESTOS
        ldr   r0,[r1,r5]
        bl    uart_print_hex
        ldr   r1,=ram_load_address
        ldr   r0,[r1,r5]
        bl    uart_print_hex
	b	.  @ loop forever upon failure ...


@ param 1 is the MMC start address to copy from (in r0)
@IMPORTAXT: the first word of BL2 should contain the length of the code to be copied
@ if the length counts the first word, then an extra word will be copied at the end,
@ since there's no checksum, this is a security risk, but who cares about security ?

.section .rodata

init_sdram_string:
.ascii "memory initialization complete ...\n\r\0"
.set init_sdram_len,.-init_sdram_string
copy_sdram_start_string:
.ascii "test copying code to dram started ...\n\r\0"
.set copy_sdram_start_len,.-copy_sdram_start_string
copy_sdram_end_string:
.ascii "test copying code to dram complete ...\n\r\0"
.set copy_sdram_end_len,.-copy_sdram_end_string
copy_sdram_err_string:
.ascii "test copying code to dram failed ...\n\r\0"
.set copy_sdram_err_len,.-copy_sdram_err_string
