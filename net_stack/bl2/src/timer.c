#include "timer.h"
#include "terminal.h"

#define DEBUG_TIM 1


void init_timer(void){
	uint32_t TCFG0_PRESCALER1_255 = (0xFD << 8); // FE + default 01 = 0xFF = 255
	uint32_t TCFG1_MUX4_16 = (0b0100 << 16);
	uint32_t TCNTB4_MAX_COUNT = 0xFFFFFFFF;

	*TCFG0	|= TCFG0_PRESCALER1_255;
	*TCFG1  |= TCFG1_MUX4_16;
	*TCNTB4  = TCNTB4_MAX_COUNT;
	
#if DEBUG_TIM
	print_format("TCFG0 = 0x%x\n\rTCFG1 = 0x%x\n\rTCNTB4 = 0x%x\n\rTCNTO4 = 0x%x\n\r",
		*TCFG0,
		*TCFG1,
		*TCNTB4,
		*TCNTO4);
#endif
	*PWMTCON   |= (1 << 21); // set update TCNTB4 and interval mode
	udelay(10);
	*PWMTCON   &= ~(1 << 21); // clear update TCNTB4
	udelay(10);
	*PWMTCON   |= (1 << 20); // start timer 4
#if DEBUG_TIM
	print_format("TCON = 0x%x\n\r",*PWMTCON);
#endif
}

/**
* returns increments in multiples of ~0.062 ms
* since timer 4 was started
*/
static uint32_t get_tick(void){
	return *TCNTO4;
}


/**
* returns increments in multiples of ~1 ms
* since base time.
* this function has an accomulating error of about 5% for every second, 
* but acceptable for timeouts in the network stack.
*/
uint32_t get_timer(uint32_t base){
	return (base == 0?(get_tick()/16UL):base - (get_tick()/16UL));
}

int init_system_timer(void){

	// enable all timer interrupt flags ...
	*(INT_CSTAT) |= 0x1;
#if DEBUG_TIM
		print_format("The INT_CSTAT register value is: 0x%x\n\r",*INT_CSTAT);	
	//	print_format("The TCFG register value is: 0x%x\n\r\0",*TCFG);	
	//	print_format("The TCON register value is: 0x%x\n\r\0",*TCON);	
#endif
	//perform software reset :
	*TCFG |= (uint32_t) TCFG_TICK_SWRST_bit;
	// wait for TCFG_TICK_SWRST_bit to auto clear ...
	while((*(TCFG) & TCFG_TICK_SWRST_bit) == TCFG_TICK_SWRST_bit){
#if DEBUG_TIM
		print_format("waiting for software timer reset ...");	
#else
		doNotOptimize();	
#endif
	}
	// set the clock source to 00 for system clock (24MHz)
	*(TCFG) |= (uint32_t) TCFG_TCLKB_MUX_bits & (0 << 13);
	// set divider and prescaler to 0
	*(TCFG) |= (uint32_t) TCFG_Divider_MUX_bits & (0 << 10);
	*(TCFG) |= (uint32_t) TCFG_Prescaler_bits & (0 << 7);

	// set tickgen sel to fractional divider (set it to 1)
	*(TCFG) |= TCFG_TICKGEN_SEL_bit;
#if DEBUG_TIM
	print_format("the value of TCFG register is:0x%x\n\r",*(TCFG));
#endif
	// set the tick integer count buffer register 
	*(TICNTB) = (uint32_t) 11; // for 1us tick interval
	// set the tick fractional count buffer register
	//*(TFCNTB) = (uint32_t) 0; // for 1us tick interval

	// wait for INT_CSTAT[2] write status bit to assert ...
	while(((*(INT_CSTAT) & INT_CSTAT_TICNTBWS_bit) == INT_CSTAT_TICNTBWS_bit)){
		// wait until this bit is set to 1 (meaning asserted) ...
		print_format("waiting for INT_CSTAT[2] to assert(0x%x) ...\n\r",*INT_CSTAT);
	}

	// set INT_CSTAT[2] to one (this is actually clearing!)
	*(INT_CSTAT) |= INT_CSTAT_TICNTBWS_bit;
	// set INT_CSTAT[3] to one (this is actually clearing!)
	//*(INT_CSTAT) &= ~INT_CSTAT_TFCNTBWS_bit;

	// wait until TICNTB & TFCNTB are set ...

	while(*(TICNTB) != (uint32_t)11 || *(TFCNTB) != 0)
		print_format("waiting to confirm integer and fractional set values\n\r");

	// starting the tick generation timer, this is done once only ...
	*(TCON) |= TCON_TIMONOFF_bit ; // setting first bit to one
	// when this function returns, the main timer generator block produces 1 tick every 1us ...


#if DEBUG_TIM
	// test to see if the timer is actually running ...
	print_format("the value of TCON register is:0x%x\n\r",*(TCON));
#endif

	return 0;
}



void udelay(int u){
	int breaker = 1000000;
	// set ICNTB register value 
	*(ICNTB) |= (u+1);
	// wait to make sure this is set, (causes longer delay??)
	while((*(INT_CSTAT) & INT_CSTAT_ICNTBWS_bit) != INT_CSTAT_ICNTBWS_bit){
		if(--breaker == 0)
			break;
	};
	*(INT_CSTAT) &= ~INT_CSTAT_ICNTBWS_bit; // clear interrupt bit ..
	*(TCON) |= TCON_INTMANUPDATE_bit; // update ICNTB
	while((*(INT_CSTAT) & INT_CSTAT_TCONWS_bit) != INT_CSTAT_TCONWS_bit){
		if(--breaker == 0)
			break;
	};

	*(INT_CSTAT) &= ~INT_CSTAT_TCONWS_bit; // clear assertion (set to 0)
	// set interrupt timer mode to one-shot:
	*(TCON) |= TCON_INTTYPE_bit ;
	while((*(INT_CSTAT) & INT_CSTAT_TCONWS_bit) != INT_CSTAT_TCONWS_bit){
		if(--breaker == 0)
			break;
	};
	*(INT_CSTAT) &= ~INT_CSTAT_ICNTBWS_bit;
	// start interrupt timer :
	*(TCON) |= TCON_INTONOFF_bit;
	while((*(INT_CSTAT) & INT_CSTAT_TCONWS_bit) != INT_CSTAT_TCONWS_bit){
		if(--breaker == 0)
			break;
	};
	*(INT_CSTAT) &= ~INT_CSTAT_ICNTBWS_bit;

	//print_format("INT_CSTAT register contains: 0x%x\n\r\0",*INT_CSTAT);
	// do nothing while the interrupt counter expired interrupt status is 0
	while((*(INT_CSTAT) & INT_CSTAT_INTCNTES_bit) != INT_CSTAT_INTCNTES_bit){
		doNotOptimize();
	};

	*(INT_CSTAT) &= ~INT_CSTAT_INTCNTES_bit; // clear assertion
	// stop interrupt timer :
	*(TCON) &= ~TCON_INTONOFF_bit;
	return;
}

