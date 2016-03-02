#define	CONFIG_SYS_HZ 	  1000

/*========= Registers for system timer ================*/

#define TCFG 0xE260000 // Timer configuration register
#define TCFG_TICK_SWRST_bit 0x00010000 // SW reset of TICK generation logic
#define TCFG_FDIV_SEL_bit 0x00008000 // Fractional divider select, set to 0 for custom, to 1 for RTC based 1ms
#define TCFG_TICKGEN_SEL_bit 0x00004000 
#define TCFG_TCLKB_MUX_bits 0x00003000 // set this to 01 for XrtcXTI, runs at 32,768KH on TINY210
#define TCFG_Divider_MUX_bits 0x00000700 // select Mux input for Timer
#define TCFG_Prescaler_bits 0x000000ff // prescaler for timer 

#define TCON 0xE2600004 // Timer Control Register
#define TCON_INTTYPE_bit 0x00000020 // 0 = One-shot, 1 = Intervals
#define TCON_INTMANUPDATE_bit 0x00000010 // interrrupt Manual update
#define TCON_INTONOFF_bit 0x00000008 // stop interrupt(0) or start interrupt(1)
#define TCON_TIMONOFF_bit 0x00000001 // stop timer(0) or start timer(1) 


#define TICNTB 0xE2600008 // Tick Integer Count Buffer register
#define TICNTO 0xE260000C // Tick Integer Count Observation Register (Read-only)
#define TFCNTB 0xE2600010 // Tick Fractional Count Buffer Register

// specific to interrupt 
#define ICNTB 0xE2600018 // Interrupt Count Buffer Register
#define ICNTO 0xE260001C // Interrrupt Count Observation Register (Read-only)
#define INT_CSTAT 0xE2600020 // Clears Interrupt (?)




int init_timer(void);
void udelay(int);
int get_timer(int);
