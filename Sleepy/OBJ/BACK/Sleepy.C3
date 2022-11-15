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


$ T16M 	ILRC, /1, BIT8;		// Use ILRC to count timer, set INTRQ.T16 = 1 every 2^9 times

$ INTEN PA0;				// set PA0 as the button and trigger the Interrupt function


// setup LED

LED				BIT PB.4;
LED_OutMode		BIT PBC.4;


BTN				BIT PA.0;
BTN_OutMode		BIT PAC.0;
BTN_Dier		BIT PADIER.0;

WORD btn_flag = 0;

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4

	//	Insert Initial Code
	set1 LED_OutMode; 		// output
	set1 LED;				// turn on LED

	set0 BTN_OutMode;		// input
	//PADIER = 0b_1111_1111;	// PADIER default is 1 for all, 1 and 2 are reserved


	WORD count = 0;


	while (1)
	{
		// blink LED to indicate that it is live
		.delay 1000*4*500; // delay is in uS, multiply by system value to ms
		set0 LED;
		.delay 1000*4*500; // delay is in uS, multiply by system value
		set1 LED;

		if (btn_flag) {
			// if button is pressed, sleep for 256
			WORD count=0;		// var to hold counter
			stt16 count; 		// reset counter to 0
			stopexe; 			// stop for 256 counts
			btn_flag = 0;		// reset flag to 0
		}


//		...
//		wdreset;

	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.PA0)
	{	//	PA0 triggered

		btn_flag = 1;
		
		
		Intrq.PA0	=	0;
		//...
	}

	popaf;
}

