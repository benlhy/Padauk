
#include	"extern.h"



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

BYTE	UART_Data_Out;

static	void	UART_Send (void)
{
	BYTE	cnt;
	

	UART_Data_Out	=	A;

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

static	BYTE	UART_Data_In;

static	void	UART_Receive (void)
{
	BYTE	cnt;

	while (1)
	{
		cnt	=	8;

		if (! UART_In)
		{
err:		//	receive UART error, so ...;
			continue;
		}
		//	Wait Start Bit
		while (UART_In) NULL;

		#if	FPPA_Duty == 1
			.Delay	(UART_Delay / 2) -2;
			if (UART_In) goto err;			//	1, 2
			.Delay	UART_Delay - 3;
			CF	=	0;						//	3
			do
			{
				t0sn	UART_In;			//	1
				CF	=	1;					//	2
				src		UART_Data_In;		//	3
				.Delay	UART_Delay - 6;
			} while (--cnt);				//	4 ~ 6
		#else
			.Delay	UART_Delay / 2;
			if (UART_In) goto err;			//	1

			.Delay	UART_Delay - 1;
			do
			{
				swapc	UART_In;			//	1
				src		UART_Data_In		//	2
				.Delay	UART_Delay - 4;
			} while (--cnt);				//	3, 4
		#endif

		A	=	UART_Data_In;			//	4
		//	Check Stop Bit
		if (! UART_In) goto err;
		return;
	}
}


// PB0 and PB1 are being used.
// PB3 ~ 7 can be used
// PA0, PA3 ~ 7 can be used
// Total number of pins: 12

WORD pOut;
int i;


void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2		//	SYSCLK=IHRC/2

	//	Insert Initial Code

	BYTE in_data;
	BYTE out_data;
	BIT state;
	BYTE temp;
	$	UART_In		In;
	$	UART_Out	High, Out;


	while (1)
	{
		$ PA.4 OUT;
		PA.4 = 1;
		.delay 100000*8;
		PA = 0b00000;
		.delay 100000*8;
		UART_Receive();

		UART_Data_Out = '1';

		UART_Send();
		
		/*
		in_data = UART_Data_In-'0';
		if(in_data<5)
		{
			temp = 1;
			do{
				temp = temp<<1;
				i++;
			}while(i<in_data);
			PA = PA | temp;
		}
		else if (in_data<12)
		{
			PB = in_data-5;
		}
		else if (in_data==13)
		{
			$ PA.0 IN;
			.delay 100; 
			if (PA.0)
			{
				UART_Data_Out = 1 + '0';
			}
			else
			{
				UART_Data_Out = '0';
			}
			UART_Send();
		}
		*/
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



