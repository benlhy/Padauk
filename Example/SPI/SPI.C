
#include	"extern.h"



void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2		//	SYSCLK=IHRC/2

	//	Insert Initial Code


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


SPI_Out			BIT	PB.2;
SPI_OutMode		BIT	_PBC.2;
SPI_In			BIT	PB.3;
SPI_InMode		BIT	_PBC.3;
SPI_Clk			BIT	PB.4;
SPI_ClkMode		BIT	_PBC.4;

static	void	SPI_Send (void)
{
	BYTE	cnt;
	BYTE	SPI_Data_Out;

	SPI_Data_Out	=	A;
	cnt	=	8;

	do
	{
		.wait1	SPI_Clk;
		sl		SPI_Data_Out;
		.wait0	SPI_Clk;
		.swapc_o SPI_Out;
	} while (--cnt);
}

static	BYTE	SPI_Data_In;

static	void	SPI_Receive (void)
{
	BYTE	cnt;
	cnt	=	8;

	do
	{
		.wait0	SPI_Clk;
		.wait1	SPI_Clk;
		#if	_SYS(OP:SWAPC IO.N)
			swapc	SPI_In;
			slc		SPI_Data_In;
		#else
			sl		SPI_Data_In;
			if (SPI_In)	SPI_Data_In.0	=	1;
		#endif
	} while (--cnt);
}

void	SPI_HandShake (void)
{
	set0	SPI_ClkMode;
	set0	SPI_InMode;
	set1	SPI_OutMode;

	while (1)
	{
		SPI_Receive();
		A	=	SPI_Data_In + 1;
		SPI_Send();
	}
}

