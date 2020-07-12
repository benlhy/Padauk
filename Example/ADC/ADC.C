
#include	"extern.h"

BYTE out;

FPPA_Duty		=>	_SYS(INC.FPPA_NUM);	// Single FPPA = 1, Mult FPPA = 2 or 4/8/...

Baud_Rate		=>	9600;
/*
UART_Delay		=>	(System_Clock / FPPA_Duty) / Baud_Rate;

	if	System_Clock	=	8,000.000 Hz
		FPPA_Duty		=	/16
	so	FPPA_Clock		=	System_Clock / FPPA_Duty	=	500.000	Hz

	if	Baud_Rate		=	19200
	so	UART_Delay		=	500.000 / 19200 = 26.0416...
	so	match, 26 cycles send one bit.	< 0.5%

	if	Baud_Rate		=	38400
	so	UART_Delay		=	500.000 / 38400 = 13.02083...
	so	match, 13 cycles send one bit.	< 0.5%

	if	Baud_Rate		=	56000
?	so	UART_Delay		=	500.000 / 56000 = 8.9285...		<	1.0%

	if	Baud_Rate		=	57600
X	so	UART_Delay		=	500.000 / 57600 = 8.6805...		:	fail
*/
UART_Delay		=>	( (System_Clock / FPPA_Duty) + (Baud_Rate/2) ) / Baud_Rate;
//	+ (Baud_Rate/2) : to round up or down

Test_V0			=>	System_Clock / 1000 * 995;
Test_V1			=>	UART_Delay * Baud_Rate * FPPA_Duty;
Test_V2			=>	System_Clock / 1000 * 1005;

#if	(Test_V1 < Test_V0) || (Test_V1 > Test_V2)
	.echo	%Test_V0 <= %Test_V1 <= %Test_V2
	.error	Baud_Rate do not match to System Clock
#endif

UART_Out		BIT	PB.0;
UART_In			BIT	PB.1;

static	void	UART_Send (void)
{
	BYTE	cnt;
	BYTE	UART_Data_Out;

	UART_Data_Out	=	out;

	//	Start Bit
	set0	UART_Out;				//	1

	#if	FPPA_Duty == 1
		cnt	=	8;						//	2 ~ 3
		.Delay	3;						//	4 ~ 6
		do
		{	//	Data Bit * 8
			.Delay	UART_Delay - 10;
			sr		UART_Data_Out;		//	7
			if (CF)
			{
				nop;					//	10
				UART_Out	=	1;		//	1
			}
			else
			{
				UART_Out	=	0;		//	1
				.delay	2;				//	2 ~ 3
			}
		} while (--cnt);				//	4 ~ 6
		.Delay	UART_Delay - 5;
	#else
		.Delay	UART_Delay - 4;
		cnt	=	8;						//	2 ~ 3

		//	Data Bit * 8
		do
		{
			sr		UART_Data_Out;		//	4		4
			swapc	UART_Out;			//			1
			.Delay	UART_Delay - 4;
		} while (--cnt);				//			2, 3

		.Delay	2;						//			3 ~ 4
	#endif

	//	Stop Bit
	set1	UART_Out;				//	1
	.Delay	2 * UART_Delay - 2;
}									//	2






void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2;		//	SYSCLK=IHRC/2

	//	Insert Initial Code
	//  UART_HandShake();
	PBC = 0b_0000_0011; // Set PB2 ~ PB3 as Input
	PBPH = 0b_0000_0000; // Disable pull high register
	PBDIER = 0b_0000_0011; // Disable digital input for PB2 ~ PB3

	$ ADCC Enable,PB3; // Enable.
	$ ADCC Enable,PB2; // The $ indicates to the compiler that 'Enable' and 'PB2' are keywords and it should look in the inc file.


	$ ADCM /16; // For system clock 8Mhz as indicated in the datasheet, clock selection
	.Delay 8*400; // delay 400us, datasheet does not state why. Probably for the pins to be set up

	// .error Hey this is an error <- nice debugging tool
	// .echo Okay done here <- Another nice tool.

	// Not actually sure which reading is used for the ADC.


	while (1)
	{
		AD_START = 1; // This is a bit in the ADCC register, write 1 to start, read 1 to see if ADC is finished.
		while(!AD_DONE) NULL; // Because AD_Done is defined as it's own 'word', so we don't need to reference the ADCC register
		
 
		out = ADCR; // ADC conversion result, the accuracy of this device is 8 bits.
		
		UART_Send(); // send out the value. Note that this is essentially a uint8 value. 
		.Delay 1000000; // don't want to flood the buffer of the receiving device.
//		...
//		wdreset;

	}
}




