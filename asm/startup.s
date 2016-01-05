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



/*after fast interrupt handler ...*/

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
 
        bl uart_asm_init

	mov r0,#0xF
        bl      flash_led

/*At this point, bl1 should load bl2 and calculate 
*the checksum for verification, then jump to execute it
*a lot needs to be done at this point, which means we 
* need to go for c ... as it would take less time to
*initialize storage devices there ...
*/




	
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

 
Undefined_Handler:
        ldr     r0,=undefined_error_string
        ldr     r1,=undefined_error_string_len
        bl      uart_print_string
        b .
SWI_Handler:
        b .
Prefetch_Handler:
        ldr     r0,=prefetch_handler_string
        ldr     r1,=prefetch_handler_string_length
        bl      uart_print_string
        b .
Data_Handler:
        ldr     r0,=data_handler_string
        ldr     r1,=data_handler_string_length
        bl      uart_print_string
        b .
IRQ_Handler:  
        b . 
           
 
 
/*==========================================
* useful routines
============================================ */
 
 
 
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



.section .rodata

undefined_error_string:
.ascii "undefined behavior exception! ...\r\n"
.set undefined_error_string_len,.-undefined_error_string
data_handler_string:
.ascii "data handler called ...\r\n"
.set data_handler_string_length,.-data_handler_string
prefetch_handler_string:
.ascii "prefetch handler called ...\r\n"
.set prefetch_handler_string_length,.-prefetch_handler_string




.end
