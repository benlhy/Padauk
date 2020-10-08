
#include	"extern.h"

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4
	
	PA = 0b_0000_0000; // Port A Data register
	PAC = 0b_1111_1111; // Port A Control register, 0/1: input/output
	PAPH = 0b_0000_0000; // Port A Pull-High Register 0/1: disable/enable
	PADIER =  0b_1111_1001; // Port A Digital Input Enable Register, 1/0: enable/disable, Bit 2:1 is reserved

	T16M = 0b_0011_0111; // Timer 16 mode: clock:CLK, Pre-divider:+64, Interrupt: bit 15

	INTEN = 0b_0000_0100;
	INTRQ = 0; // Clear the INTRQ register
    ENGINT;    // Enable global interrupts

	//	Insert Initial Code

	while (1)
	{

	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.T16)
	{	//	T16 Trig
			
		Intrq.T16	=	0;

		// does not do conversions from binary to values, i.e pa.3 = !pa.3 doesn't work

		if (pa.3 == 1) {
			pa.3 = 0;
		} else {
			pa.3 = 1;
		}
	}

	popaf;
}

