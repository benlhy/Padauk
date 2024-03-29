
/*
	Capcitive Sensor - based off Touchpad controller 

	IC: PFS173

	Version: 1.1

	Description: This code uses Port B as simple polling touch controllers.
	It is not timing optimized, but it does the job pretty well - detecting
	touch inputs. Port A is tied to the LED output to indicate when a value has
	been touched.

	It has been improved from the original touch controller by capturing the initial value
	before adding a margin to detect a touch

	The touch threshold has been optimized for a 1M resistor 

	It scans 3 pins and if a touch is detected on any of the pins it will send a high value.

	Note that PA.5 needs a pullup resistor.

	1.1 Updated capsense with sleep between sensing events using stopexe

	Not sure if sleep will affect timing of capsense


*/


#include	"extern.h"



TOUCH1			BIT PB.6;
TOUCH1_OutMode	BIT PBC.6;
READ1			BIT PB.5;
READ1_OutMode	BIT PBC.5;
LED				BIT PB.4;
LED_OutMode		BIT PBC.4;

WORD end;

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4

	$ T16M 	ILRC, /64, BIT12;	// Use ILRC/64 to count timer, set INTRQ.T16 = 1 every 2^15 times
								// ILRC approx 95kHz therefore trigger every 21 seconds
	$ INTEN T16; 			// set T16 as  the for the Interrupt function
	$ INTEGS BIT_R; 		// edge select
	INTRQ = 0; 				// clear intrq
	ENGINT; 				// enable global interrupts

	// Port A setup

	PADIER = 0b_0111_0000;  // Port A Digital Input Enable Register, 1/0:
                       	    // enable/disable, Bit 2:1 is reserved

	PAPH = 0b_0000_0000;	// Port A pull high register

	// Port B setup

	PBDIER = 0b_0111_1111;  // Port B Digital Input Enable Register, 1/0:
							// enable/disable

	PBPH = 0b_0000_0001;	// Port B pull high register

	
	// Timer 2 setup
	TM2C = 0b_0001_00_0_0;	// CLK, disabled, period mode, disable polarity inversion
	TM2B = 0b_0111_1111;	// count+1/256 * 100 = %
	TM2S = 0b_0_10_00000;	// 8 bit PWM, /4 prescalar


	set0	READ1_OutMode;	// set READ1 to input
	set1	LED_OutMode;
	PAC = 0x0;				// set all A to outputs for debugging purposes
	PA = 0xFF;			 	// turn on all A


	WORD baseline_4 = 0;
	WORD baseline_5 = 0;
	WORD baseline_6 = 0;

	WORD margin = 0x10; // margin of difference = 16 counts

	// baseline calibration for dry/untouched status

	BYTE j = 0;

	.FOR ww, <4,5,6>
		j = 40; // iterate 8x more to average
		end = 0;
		do {
			// set output for PB.x and set it to high
			set1 PBC.ww;
			set1 PB.ww;
			.delay 1000*4; 			// .delay is in uS, need to multiply by sysclk Mhz, charging seq
			set0 PBC.ww;		 	// set pin input - discharge
			TM2CT = 0; 				// start counting
			.wait0 PB.ww;			// wait for read to go low
			end = end + TM2CT;		// end
		
		} while (j--);
		 end = end>>2; // divide by 8
		baseline_#ww = end;

	.ENDM

	while (1)
	{

		BYTE i = 10;
		WORD comparison = 0;
		WORD touch_flag = 0;
		// for ww = 4,5,6

		.FOR ww, <4,5,6>
			i = 10; // decrement i
			do {
				// set output for PBC and PB
				set1 PBC.ww;
				set1 PB.ww;
				.delay 1000*4; 			// .delay is in uS, need to multiply by sysclk Mhz, charging seq
				set0 PBC.ww;		 	// set pin input
				TM2CT = 0; 				// start counting
				.wait0 PB.ww;			// wait for read to go low
				end = end + TM2CT;		// end
			} while (i--);

			comparison = baseline_#ww + margin; // set the area to compare with

			if (end > comparison) { // if the count is > baseline + margin -> we have a positive event
				touch_flag = 1;
			} 
			
			end = 0; // reset value 
		.ENDM

		if (touch_flag == 0) {
			set1 PAC.4; 			// turn off pin 4 for only if touch_flag not set event
			set0 PA.4;

			// sleep only if there is no active event
			WORD count = 0 ;		// var to hold counter value
			stt16 count;			// reset T16 timer to 0

			CLKMD = 0xF4;			// switch clock to ILRC
			CLKMD.4 = 0;			// turn off IHRC
			stopexe;				// stop system clock
			CLKMD.4 = 1;			// enable IHRC
			CLKMD = 0x14;			// set clock to IHRC/4

			.delay 1000*4*33; 		// wait 33 miliseconds for debounce
			

		} else {
			set1 PAC.4;
			set1 PA.4;
			.delay 1000*4*33;
		}
		

	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.T16)
	{	//	T16 Trig
		
		Intrq.T16	=	0;
		//...
	}

	popaf;
}

