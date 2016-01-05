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

.text
.code 32
.global clock_subsys_init


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



.section .rodata
init_clock_string:
.ascii "Clock System initialization complete ...\r\n"
.set init_clock_len,.-init_clock_string

