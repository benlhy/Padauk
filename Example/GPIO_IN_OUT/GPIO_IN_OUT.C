
#include	"extern.h"

// PA0: Input, PA3: Output

void	FPPA0 (void)
{
	.ADJUST_IC SYSCLK=IHRC/2;
	PA = 0b_0000_0000; // Port A Data register
	PAC = 0b_1111_1110; // Port A Control register, 0/1: input/output
	PAPH = 0b_0000_0001; // Port A Pull-High Register 0/1: disable/enable
	$ PADIER 0b_1111_1001; // Port A Digital Input Enable Register, 1/0: enable/disable, Bit 2:1 is reserved
	while (1)
	{
		if( pa.0 == 0)
		{
			pa.3 = 1;

		}
		else 
		{
			pa.3 = 0;
		}
//		...
//		wdreset;

	}
}

