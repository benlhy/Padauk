/*
	Project Name: 	Sleepy
	IC:				PFS173
	Description:	This is a test of the Padauk's IC sleep capability and power draw.
					It blinks an LED until a button is pressed, then it will trigger an interrupt
					that counts for 256 counts.
	Version:		1
	Date: 			15 Nov 2022

	Pins used:		PA0, PB4
*/


#include	"extern.h"



$ INTEN PA0;				


// setup LED

LED				BIT PB.4;
LED_OutMode		BIT PBC.4;


BTN				BIT PA.0;
BTN_OutMode		BIT PAC.0;
BTN_Dier		BIT PADIER.0;

WORD btn_flag = 0;

void	FPPA0 (void)
{
	
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4, IHRC is 16Mhz - SYSCLK is 4Mhz
	
	$ T16M 	IHRC, /64, BIT15;	// Use ILRC to count timer, set INTRQ.T16 = 1 every 2^13 times
								// ILRC approx 95kHz therefore trigger every 0.68 seconds
	$ INTEN PA0; 		// set PA0 as the button and trigger the Interrupt function
	$ INTEGS BIT_R, PA0_F; // edge select
	INTRQ = 0; // clear intrq
	ENGINT; // enable global interrupt s
	
	//	Insert Initial Code
	set1 LED_OutMode; 		// output
	set1 LED;				// turn on LED

	set0 BTN_OutMode;		// input
	PADIER = 0b_0000_0001;	// PADIER default is 1 for all, 1 and 2 are reserved, disable others to 
							// prevent leakage


	while (1)
	{
		// blink LED to indicate that it is live
		.delay 1000*4*500; 		// delay is in SYSCLK counts 4000/4000,000 s = 1/1000 s = 1ms
		set0 LED;
		.delay 1000*4*500; 		// 500ms
		set1 LED;

		if (btn_flag) {
			// if button is pressed, sleep for approx 0.6 seconds before resume execution
			set0 LED;
			.delay 1000*4;

			// how to save more power?
			// switch system clock to ILRC
			//CLKMD = 0b11110110; // switch system clock to ILRC
			//CLKMD.4 = 0; 		// disable IHRC 

			WORD count=0;		// var to hold counter
			stt16 count; 		// reset counter to 0
			stopexe; 			// stop execution
			//CLKMD.4 = 1;		// enable IHRC
			//CLKMD = 0b00010110;	// switch system clock to IHRC 

			btn_flag = 0;		// reset flag to 0

		}


	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.PA0)
	{	//	PA0 triggered

		btn_flag = 1; // set flag
		
		
		Intrq.PA0	=	0; //clear interrupt
		//...
	}

	popaf;
}

