
/*
	Touchpad controller 

	IC: PFS173

	Version: 1

	Description: This code uses Port B as simple polling touch controllers.
	It is not very timing optimized, but it does the job pretty well - detecting
	touch inputs. Port A is tied to the LED output to indicate when a value has
	been touched.

	The touch threshold has been optimized for a 1M resistor 

	TODO:
	1. Clean up vestigial code

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

	//	Insert Initial Code

	// Port A setup

	PADIER = 0b_0000_0000;  // Port A Digital Input Enable Register, 1/0:
                       	    // enable/disable, Bit 2:1 is reserved

	PAPH = 0b_0000_0000;	// Port A pull high register

	// Port B setup

	PBDIER = 0b_0110_0001;  // Port B Digital Input Enable Register, 1/0:
							// enable/disable

	PBPH = 0b_0000_0001;	// Port B pull high register

	
	// Timer 2 setup
	TM2C = 0b_0001_00_0_0;	// CLK, disabled, period mode, disable polarity inversion
	TM2B = 0b_0111_1111;	// count+1/256 * 100 = %
	TM2S = 0b_0_10_00000;	// 8 bit PWM, /4 prescalar


	set0	READ1_OutMode;	// set READ1 to input
	set1	LED_OutMode;
	PAC = 0xFF;				// set all A to outputs for debugging purposes


	while (1)
	{
		BYTE i = 10;
		.FOR ww, <4,5,6>
			i = 10;
			do {
				set1 PBC.ww;
				set1 PB.ww;
				//set1 TOUCH1_OutMode;	// set pin output
				//set1 TOUCH1; 			// set pin high
				.delay 1000*4; 			// .delay is in uS, need to multiply by sysclk Mhz, charge
				set0 PBC.ww;		 	// set pin input
				TM2CT = 0; 				// start
				.wait0 PB.ww;			// wait for read to go low
				end = end + TM2CT;		// end
			} while (i--);
			

			if (end > 0x180) {
				// touch detected
				//set1 LED;
				set1 PA.ww
			} else {
				//set0 LED;
				set0 PA.ww
			}
			end = 0;
		.ENDM

//		...
//		wdreset;

	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.T16)
	{	//	T16 Trig
		//	User can add code
		Intrq.T16	=	0;
		//...
	}

	popaf;
}

