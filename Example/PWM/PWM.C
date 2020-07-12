
#include	"extern.h"



void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/16, IHRC = 16MHz, VDD=5V;		//	SYSCLK=IHRC/2

	Byte duty = 60;
	Byte _duty = 100-duty;
	Byte increment = 1;

	PWMG0DTL = 0x00; //PWM0 Duty Value Low Register * You have to write to the low register first [2:1]
	PWMG0DTH = _duty; //PWM0 Duty Value HIGH Register [10:3]
	PWMGCUBL = 0x00; //PWM0 Counter Upper Bound Low Register, Bits 2:1
	PWMGCUBH = 100; //PWM0 Count Upper Bound High Register, Bits 10:3 

	$ PWMG0C Enable,Inverse,PA0; 
	$ PWMGCLK Enable,/1,SYSCLK;


	//	Insert Initial Code

	while (1)

	{ 
		PWMG0DTL = 0x00;
		PWMG0DTH = duty;
		
		if (duty==100)
		{
			duty=100;
			increment=-1;
		}
		else if(duty==0) // it cannot compare negative values.
		{
			duty=0;
			increment=1;
		}
		else
		{
			nop;
		}
		duty = duty + increment;
		.delay 10000;
		
	}
}




