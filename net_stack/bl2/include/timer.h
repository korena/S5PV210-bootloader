#define	CONFIG_SYS_HZ 	  1000

/*========= Registers for system timer ================*/

#define TCFG (volatile uint32_t*) 0xE2600000 // Timer configuration register
#define TCFG_TICK_SWRST_bit 0x00010000 // SW reset of TICK generation logic
#define TCFG_FDIV_SEL_bit 0x00008000 // Fractional divider select, set to 0 for custom, to 1 for RTC based 1ms
#define TCFG_TICKGEN_SEL_bit 0x00004000 
#define TCFG_TCLKB_MUX_bits 0x00003000 // set this to 01 for XrtcXTI, runs at 32,768KH on TINY210
#define TCFG_Divider_MUX_bits 0x00000700 // select Mux input for Timer
#define TCFG_Prescaler_bits 0x000000ff // prescaler for timer 

#define TCON (volatile uint32_t*)0xE2600004 // Timer Control Register
#define TCON_INTTYPE_bit 0x00000020 // 0 = One-shot, 1 = Intervals
#define TCON_INTMANUPDATE_bit 0x00000010 // interrrupt Manual update
#define TCON_INTONOFF_bit 0x00000008 // stop interrupt(0) or start interrupt(1)
#define TCON_TIMONOFF_bit 0x00000001 // stop timer(0) or start timer(1) 


#define TICNTB (volatile uint32_t*)0xE2600008 // Tick Integer Count Buffer register
#define TICNTO (volatile uint32_t*)0xE260000C // Tick Integer Count Observation Register (Read-only)
#define TFCNTB (volatile uint32_t*)0xE2600010 // Tick Fractional Count Buffer Register

// specific to interrupt 
#define ICNTB (volatile uint32_t*)0xE2600018 // Interrupt Count Buffer Register
#define ICNTO (volatile uint32_t*)0xE260001C // Interrrupt Count Observation Register (Read-only)
#define INT_CSTAT (uint32_t*)0xE2600020 // Clears Interrupt (?)
#define INT_CSTAT_TWIE_bit 0x00000400 // TCON Write Interrupt Enable
#define INT_CSTAT_IWIE_bit 0x00000200 // ICNTB write Interrupt Enable
#define INT_CSTAT_TFWIE_bit 0x00000100 // TFCNTB write Int Enable
#define INT_CSTAT_TIWIE_bit 0x00000080 // TICNTB write Int Enable
#define INT_CSTAT_ICNTEIE_bit 0x00000040 // Int count expired Enable
#define INT_CSTAT_TCONWS_bit 0x00000020 // TCON write status
#define INT_CSTAT_ICNTBWS_bit 0x00000010 // ICNTB write status
#define INT_CSTAT_TFCNTBWS_bit 0x00000008 // TFCNTB write status
#define INT_CSTAT_TICNTBWS_bit 0x00000004 // TICNTB write status
#define INT_CSTAT_INTCNTES_bit 0x00000002 // interrrupt conunter expired status (INTCNT=0)
#define INT_CSTAT_INTENBL_bit 0x00000001 // interrupt enable bit 


int init_timer(void);
void udelay(int);
int get_timer(int);
extern void doNotOptimize(void);
