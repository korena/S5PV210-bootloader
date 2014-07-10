/* Standard definitions of mode bits and interrupt (I & F) flags in PSRs */
.equ Mode_USR,   0x10
.equ Mode_FIQ,   0x11
.equ Mode_IRQ,   0x12
.equ Mode_SVC,   0x13
.equ Mode_ABT,   0x17
.equ Mode_UND,   0x1B
.equ Mode_SYS,   0x1F
 
/*useful addresses, fetched from the S5PV210 user manual*/
.equ GPIO_BASE,  0xE0200000
.equ GPJ2CON_OFFSET,  0x280   
.equ GPJ2DAT_OFFSET,   0x284
.equ GPJ2PUD_OFFSET,   0x288
.equ GPJ2DRV_SR_OFFSET,  0x28C
.equ GPJ2CONPDN_OFFSET,  0x290
.equ GPJ2PUDPDN_OFFSET,  0x294
 
 
/*clock configuration registers */
.equ ELFIN_CLOCK_POWER_BASE,  0xE0100000
.equ CLK_SRC6_OFFSET,  0x218
.equ CLK_SRC0_OFFSET,  0x200
 
.equ APLL_CON0_OFFSET,  0x100
.equ APLL_CON1_OFFSET,  0x104
.equ MPLL_CON_OFFSET,  0x108
.equ EPLL_CON_OFFSET,  0x110
.equ VPLL_CON_OFFSET,  0x120
 
.equ CLK_DIV0_OFFSET,  0x300
.equ CLK_DIV0_MASK,  0x7fffffff
.equ CLK_DIV6_OFFSET,  0x318
 
.equ CLK_OUT_OFFSET,  0x500
 
 
/* PMS values constants*/
.equ APLL_MDIV,   0x7D  @125
.equ APLL_PDIV,   0x3   @3
.equ APLL_SDIV,   0x1   @1
 
.equ MPLL_MDIV,   0x29b @667
.equ MPLL_PDIV,   0xc   @12
.equ MPLL_SDIV,   0x1   @1
 
.equ EPLL_MDIV,   0x60  @96
.equ EPLL_PDIV,   0x6   @6
.equ EPLL_SDIV,   0x2   @2
 
.equ VPLL_MDIV,   0x6c  @108
.equ VPLL_PDIV,   0x6   @6
.equ VPLL_SDIV,   0x3   @3
 
/*the next places MDIV value at address 16, PDIV at address 8, SDIV at address 0 of the CLK_DIV0 register, it also sets the highest bit to turn on the APLL,*/
.equ APLL_VAL,    ((1<<31)|(APLL_MDIV<<16)|(APLL_PDIV<<8)|(APLL_SDIV))
.equ MPLL_VAL,    ((1<<31)|(MPLL_MDIV<<16)|(MPLL_PDIV<<8)|(MPLL_SDIV))
.equ EPLL_VAL,    ((1<<31)|(EPLL_MDIV<<16)|(EPLL_PDIV<<8)|(EPLL_SDIV))
.equ VPLL_VAL,    ((1<<31)|(VPLL_MDIV<<16)|(VPLL_PDIV<<8)|(VPLL_SDIV))
/* Set AFC value */
.equ AFC_ON,    0x00000000
.equ AFC_OFF,    0x10000010
 
 
 
/* CLK_DIV0 constants*/
.equ APLL_RATIO,  0
.equ A2M_RATIO,   4
.equ HCLK_MSYS_RATIO,  8
.equ PCLK_MSYS_RATIO,  12
.equ HCLK_DSYS_RATIO,  16
.equ PCLK_DSYS_RATIO,  20
.equ HCLK_PSYS_RATIO,  24
.equ PCLK_PSYS_RATIO,  28
 
.equ CLK_DIV0_VAL,      ((0<<APLL_RATIO)|(4<<A2M_RATIO)|(4<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))

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

.equ UART_CONSOLE_BASE, (UART_BASE + UART0_OFFSET)



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
 
.equ I_Bit,      0x80 /* when I bit is set, IRQ is disabled*/
.equ F_Bit,      0x40 /* when F bit is set, FIQ is disabled*/
 
.text
.code 32
.global _start
 
_start:
        b Reset_Handler
        b Undefined_Handler
        b SWI_Handler
        b Prefetch_Handler
        b Data_Handler
        nop /* Reserved vector*/
        b IRQ_Handler
/*FIQ handler would go right here ...*/
 
.globl _bss_start
_bss_start:
 .word bss_start
 
.globl _bss_end
_bss_end:
 .word bss_end
 
.globl _data_start
_data_start:
 .word data_start
 
.globl _rodata
_rodata:
 .word rodata
 
Reset_Handler:
  
/* set the cpu to SVC32 mode and disable IRQ & FIQ */
         msr CPSR_c, #Mode_SVC|I_Bit|F_Bit ;
       /* Disable Caches */
        mrc p15, 0, r1, c1, c0, 0 /* Read Control Register configuration data*/
        bic r1, r1, #(0x1 << 12)  /* Disable I Cache*/
        bic r1, r1, #(0x1 << 2)   /* Disable D Cache*/
        mcr p15, 0, r1, c1, c0, 0 /* Write Control Register configuration data*/
 
        /* Disable L2 cache (too specific, not needed now, but useful later)*/
        mrc p15, 0, r0, c1, c0, 1  /* reading auxiliary control register*/
        bic r0, r0, #(1<<1)
        mcr p15, 0, r0, c1, c0, 1  /* writing auxiliary control register*/
 
        /* Disable MMU */
        mrc p15, 0, r1, c1, c0, 0 /* Read Control Register configuration data*/
        bic r1, r1, #0x1
        mcr p15, 0, r1, c1, c0, 0 /* Write Control Register configuration data*/
 
        /* Invalidate L1 Instruction cache */
        mov r1, #0
        mcr p15, 0, r1, c7, c5, 0
 
        /*Invalidate L1 data cache and L2 unified cache*/
        bl invalidate_unified_dcache_all
 
        /*enable L1 cache*/
        mrc     p15, 0, r1, c1, c0, 0 /* Read Control Register configuration data*/
        orr     r1, r1, #(0x1 << 12)  /* enable I Cache*/
        orr     r1, r1, #(0x1 << 2)   /* enable D Cache*/
        mcr     p15, 0, r1, c1, c0, 0 /* Write Control Register configuration data*/
 
        /*enable L2 cache (in addition to I,D cache on all levels)*/
        mrc     p15, 0, r0, c1, c0, 1
        orr     r0, r0, #(1<<1)
        mcr     p15, 0, r0, c1, c0, 1
      
 
        ldr sp, =0xd0037d80 /* SVC stack top, from irom documentation*/
        sub sp, sp, #12 /* set stack */
       @mov fp, #0
 
        ldr r0,=0x0C
        bl flash_led
 
        bl clock_subsys_init

        ldr r0,=0x0F
        bl flash_led
        bl uart_asm_init

        ldr r0,=0x0C
        bl flash_led

        bl mem_ctrl_asm_init

        ldr r0,=0x0F
        bl flash_led

        /* copy a block of code from read only memory to ram,
         * and jump to execute it, the executed code should give a
         * certain message if successful*/

        bl      copy_To_Mem
        ldr     r0,=0x0C  @ corrupt r0
        ldr     ip,=ram_load_address
        mov     lr, pc
        bx      ip

 _end:
        bl      flash_led
        b       _end

 
Undefined_Handler:
        b .
SWI_Handler:
        b .
Prefetch_Handler:
        b .
Data_Handler:
        b .
IRQ_Handler:  
        b . 
           
 
 
/*==========================================
* useful routines
============================================ */
 
 
/*clock subsystem initialization code*/
clock_subsys_init:
 
         ldr r0, =ELFIN_CLOCK_POWER_BASE @0xE0100000
 
         ldr r1, =0x0
         str r1, [r0, #CLK_SRC0_OFFSET]
 
         ldr r1, =0x0
         str r1, [r0, #APLL_CON0_OFFSET]
         ldr r1, =0x0
         str r1, [r0, #MPLL_CON_OFFSET]
         ldr r1, =0x0
         str r1, [r0, #MPLL_CON_OFFSET]
  
 /*turn on PLLs and set the PMS values according to the recommendation*/
         ldr r1, =APLL_VAL
         str r1, [r0, #APLL_CON0_OFFSET]
 
         ldr r1, =MPLL_VAL
         str r1, [r0, #MPLL_CON_OFFSET]
 
         ldr r1, =VPLL_VAL
         str r1, [r0, #VPLL_CON_OFFSET]
 
         ldr r1, =AFC_ON
         str r1, [r0, #APLL_CON1_OFFSET]
 
         ldr r1, [r0, #CLK_DIV0_OFFSET]
         ldr r2, =CLK_DIV0_MASK
         bic r1, r1, r2
 
         ldr r2, =CLK_DIV0_VAL
         orr r1, r1, r2
         str r1, [r0, #CLK_DIV0_OFFSET]
 
    /*delay for the PLLs to lock*/
         mov r1, #0x10000
1:       subs r1, r1, #1
         bne  1b
     
 /* Set Mux to PLL (Bus clock) */
 
 /* CLK_SRC0 PLLsel -> APLLout(MSYS), MPLLout(DSYS,PSYS), EPLLout, VPLLout (glitch free)*/
         ldr r1, [r0, #CLK_SRC0_OFFSET]
         ldr r2, =0x10001111
         orr r1, r1, r2
         str r1, [r0, #CLK_SRC0_OFFSET]
 
 /* CLK_SRC6[25:24] -> MUXDMC0 clock select = SCLKMPLL (which is running at 667MHz, needs to be divided to a value below 400MHz)*/
         ldr r1, [r0, #CLK_SRC6_OFFSET]
         bic r1, r1, #(0x3<<24)
         orr r1, r1, #0x01000000
         str r1, [r0, #CLK_SRC6_OFFSET]
 
 /* CLK_DIV6[31:28] -> SCLK_DMC0 = MOUTDMC0 / (DMC0_RATIO + 1) -> 667/(3+1) = 166MHz*/
         ldr r1, [r0, #CLK_DIV6_OFFSET]
         bic r1, r1, #(0xF<<28)
         orr r1, r1, #0x30000000
         str r1, [r0, #CLK_DIV6_OFFSET]
 
        /*the clock output routes on of the configured clocks to an output pin, if you have a debugger to
         * verify the outcome of your configuration, I am at home, and have no access to such hardware at the moment of writing. 
        */
 /* CLK OUT Setting */
 /* DIVVAL[23:20], CLKSEL[16:12] */
         ldr r1, [r0, #CLK_OUT_OFFSET]
         ldr r2, =0x00909000
         orr r1, r1, r2
         str r1, [r0, #CLK_OUT_OFFSET]
 
         mov pc, lr
 
/*Massive data/unified cache cleaning to the point of coherency routine, loops all available levels!*/
 
clean_unified_dcache_all:
         mrc p15, 1, r0, c0, c0, 1 /* Read CLIDR into R0*/
         ands r3, r0, #0x07000000
         mov r3, r3, lsr #23 /* Cache level value (naturally aligned)*/
         beq Finished
         mov r10, #0
Loop1:
         add r2, r10, r10, lsr #1 /* Work out 3 x cache level*/
         mov r1, r0, lsr r2 /* bottom 3 bits are the Cache type for this level*/
         and r1, r1, #7 /* get those 3 bits alone*/
         cmp r1, #2
         blt Skip /* no cache or only instruction cache at this level*/
         mcr p15, 2, r10, c0, c0, 0 /* write CSSELR from R10*/
         isb /* ISB to sync the change to the CCSIDR*/
         mrc p15, 1, r1, c0, c0, 0 /* read current CCSIDR to R1*/
         and r2, r1, #7 /* extract the line length field*/
         add r2, r2, #4 /* add 4 for the line length offset (log2 16 bytes)*/
         ldr r4, =0x3FF
         ands r4, r4, r1, lsr #3 /* R4 is the max number on the way size (right aligned)*/
         clz r5, r4 /* R5 is the bit position of the way size increment*/
         mov r9, r4 /* R9 working copy of the max way size (right aligned)*/
Loop2:
         ldr r7, =0x00007FFF
         ands r7, r7, r1, lsr #13 /* R7 is the max num of the index size (right aligned)*/
Loop3:
         orr r11, r10, r9, lsl R5 /* factor in the way number and cache number into R11*/
         orr r11, r11, r7, lsl R2 /* factor in the index number*/
         mcr p15, 0, r11, c7, c10, 2 /* DCCSW, clean by set/way*/
         subs r7, r7, #1 /* decrement the index*/
         bge Loop3
         subs r9, r9, #1 /* decrement the way number*/
         bge Loop2
Skip:
         add r10, r10, #2 /* increment the cache number*/
         cmp r3, r10
         bgt Loop1
         dsb
Finished:
         mov pc, lr
 
 
 
/*Massive data/unified cache invalidation, loops all available levels!*/
invalidate_unified_dcache_all:
         mrc p15, 1, r0, c0, c0, 1 /* Read CLIDR into R0*/
         ands r3, r0, #0x07000000
         mov r3, r3, lsr #23 /* Cache level value (naturally aligned)*/
beq Finished_
         mov r10, #0
Loop_1:
         add r2, r10, r10, lsr #1 /* Work out 3 x cache level*/
         mov r1, r0, lsr r2 /* bottom 3 bits are the Cache type for this level*/
         and r1, r1, #7 /* get those 3 bits alone*/
         cmp r1, #2
         blt Skip_ /* no cache or only instruction cache at this level*/
         mcr p15, 2, r10, c0, c0, 0 /* write CSSELR from R10*/
         isb /* ISB to sync the change to the CCSIDR*/
         mrc p15, 1, r1, c0, c0, 0 /* read current CCSIDR to R1*/
         and r2, r1, #7 /* extract the line length field*/
         add r2, r2, #4 /* add 4 for the line length offset (log2 16 bytes)*/
         ldr r4, =0x3FF
         ands r4, r4, r1, lsr #3 /* R4 is the max number on the way size (right aligned)*/
         clz r5, r4 /* R5 is the bit position of the way size increment*/
         mov r9, r4 /* R9 working copy of the max way size (right aligned)*/
Loop_2:
         ldr r7, =0x00007FFF
         ands r7, r7, r1, lsr #13 /* R7 is the max num of the index size (right aligned)*/
Loop_3:
         orr r11, r10, r9, lsl R5 /* factor in the way number and cache number into R11*/
         orr r11, r11, r7, lsl R2 /* factor in the index number*/
         mcr p15, 0, r11, c7, c6, 2 /* Invalidate line described by r11*/
         subs r7, r7, #1 /* decrement the index*/
         bge Loop_3
         subs r9, r9, #1 /* decrement the way number*/
         bge Loop_2
Skip_:
         add r10, r10, #2 /* increment the cache number*/
         cmp r3, r10
         bgt Loop_1
         dsb
Finished_:
         mov pc, lr
 
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
uart_print_hex:
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
	mov	pc,lr

mem_ctrl_asm_init:
		stmfd sp!,{r4-r11, lr}
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


	ldr	r1, =0x20F01422				@ MemConfig0 512MB config, 4 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 
	str	r1, [r0, #DMC_MEMCONFIG0]		@ 2:Mixed 
							@ 0x20F01422 (0010-0000-1111-0000-0000-0011-0010-0010) 0x20E01323
					                @ 4 banks [3:0] = 0x2, row address bits = 14 => [4:7] = 0x2 (found in K4T1G084QF 5. 
	                                                @ DDR2 SDRAM Addressing),col address bits = 11 => [11:8] = 0x4 (I think!), memory  
	                                                @ address scheme  explained in 5.3 Address Mapping Scheme [15:12] = 0x0.
	                                                @ [23:16] kept at default, [31:24] also kept as default. 

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

	mov r1,#init_sdram_len

	bl  uart_print_string


	ldmfd sp!,{r4-r11, pc}

 
.align 4,0x90
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

.align 4,0x90
copy_To_Mem:
        stmfd sp!,{r4-r11,lr}

        ldr     r0,=copy_sdram_start_string
        ldr     r1,=copy_sdram_start_len
        bl      uart_print_string

        ldr     r0,=_BL2
        ldr     r1,=ram_load_address
        mov     r2,#copy_lim
1:      ldr     r3,[r0],#4
        str     r3,[r1],#4
        subs    r2,r2,#1
        bne     1b
	/*test the first and last words of the copied segment, if they match, assume successful*/
	
	ldr	r3,=_BL2
	ldr	r0,[r3]
	ldr     r1,=ram_load_address
	ldr	r2,[r1]  @ the contents of the ram load address.
	cmp	r0,r2 @ This is test one, if not equal, go to exit_copy.
	bne	exit_copy
	ldr	r0,[r3,#copy_lim]	
	ldr	r4,[r1,#copy_lim]
	cmp	r0,r4 @ This is test two, if not equal, go to exit_copy.
        ldr     r0,=copy_sdram_end_string
        ldr     r1,=copy_sdram_end_len
        bl      uart_print_string
	ldmfd sp!,{r4-r11,pc}
exit_copy:
        ldr   r0,=copy_sdram_err_string
        ldr   r1,=copy_sdram_err_len
        bl    uart_print_string
	ldr   r1,=_BL2
	ldr   r0,[r1]
	bl    uart_print_hex
	ldr   r1,=ram_load_address
	ldr   r0,[r1]
	bl    uart_print_hex

        ldr   r1,=_BL2
        ldr   r0,[r1,#copy_lim]
        bl    uart_print_hex
        ldr   r1,=ram_load_address
        ldr   r0,[r1,#copy_lim]
        bl    uart_print_hex
	b	.  @ loop forever upon failure ...

.section .rodata
init_uart_string:
.ascii "UART 0 Initialization complete ...\r\n"
.set init_uart_len,.-init_uart_string
init_clock_string:
.ascii "Clock System initialization complete ...\r\n"
.set init_clock_len,.-init_clock_string
init_sdram_string:
.ascii "memory initialization complete ...\r\n"
.set init_sdram_len,.-init_sdram_string
copy_sdram_start_string:
.ascii "copying code to dram started ...\r\n"
.set copy_sdram_start_len,.-copy_sdram_start_string
copy_sdram_end_string:
.ascii "copying code to dram complete ...\r\n"
.set copy_sdram_end_len,.-copy_sdram_end_string
copy_sdram_err_string:
.ascii "copying code to dram failed ...\r\n"
.set copy_sdram_err_len,.-copy_sdram_err_string


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
.ascii "code execution from dram successful! ...\r\n"
.set exec_sdram_len,.-exec_sdram_string
.align 4
.set copy_lim,.-_BL2
