
#include	"extern.h"




void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4

	//	Insert Initial Code

	$ RFCC PB6,START,C_TYPE,OUTPUT;



	while (1)
	{
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

