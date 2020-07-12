// This source code shows how to read and write over I2C to the Padauk IC.
// This is useful for extracting data.


#include	"extern.h"


void I2C_Slave(void);

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2		//	SYSCLK=IHRC/2

	//	Insert Initial Code


	while (1)
	{
//		...
//		wdreset;
		I2C_Slave();

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


I2C_SDA_Slave		BIT	PB.5;
I2C_SCL_Slave		BIT	PB.4;

I2C_DEVICE	EQU	0x00		//	Please modify it by your device
							//	Because only 24Cxx use the 0xA0 for device code.
DEVICE_LEN	EQU	4			//	Only need compare Bit 7 ~ Bit 4
							//	Bit 3 ~ Bit 1 is for high address
.errz	DEVICE_LEN <= 7		//	must check 1 <= DEVICE_LEN <= 7
.errz	DEVICE_LEN >= 1

void	I2C_Slave (void)
{
	$ I2C_SCL_Slave	In;
	$ I2C_SDA_Slave	In;

	.delay	99;		//	Wait for master ready

	WORD	addr;	//
	addr$1	=	0;	//	at the sample code, only access one byte of address.

Stop:	.wait1	I2C_SCL_Slave
		if (! I2C_SDA_Slave)	goto Stop;

High:	if (! I2C_SCL_Slave)	goto Stop;
		if (I2C_SDA_Slave)		goto High;

Start:	if (! I2C_SCL_Slave)	goto Chk_Ax;
		if (! I2C_SDA_Slave)	goto Start;
		goto High;

Chk_Ax:
	BYTE	count	=	DEVICE_LEN; // 4
	A	=	I2C_DEVICE; // 0XA0
	do
	{
		sl	A;
		.wait1	I2C_SCL_Slave
		if (I2C_SDA_Slave)
		{
			if (! CF) goto Stop;
			while (1)
			{
				if (! I2C_SCL_Slave)	break;
				if (! I2C_SDA_Slave)	goto Start;
			}
		}
		else
		{
			if (CF) goto Stop;
			while (1)
			{
				if (! I2C_SCL_Slave)	break;
				if (I2C_SDA_Slave)		goto Stop;
			}
		}
	} while (--count);

	if (DEVICE_LEN != 7)
	{
		BYTE	hi_adr	=	0;
		A	=	7 - DEVICE_LEN; // Truncate?
		do
		{
			.wait1	I2C_SCL_Slave
			if (I2C_SDA_Slave)
			{
				CF	=	1;
				slc	hi_adr;
				while (1)
				{
					if (! I2C_SCL_Slave)	break;
					if (! I2C_SDA_Slave)	goto Start;
				}
			}
			else
			{
				sl	hi_adr;
				while (1)
				{
					if (! I2C_SCL_Slave)	break;
					if (I2C_SDA_Slave)		goto Stop;
				}
			}
		} while (--A);
	}

	.wait1	I2C_SCL_Slave

	BYTE	data;

	if (I2C_SDA_Slave)
	{	//	Read
		data = 1;
		while (1)
		{
			if (! I2C_SCL_Slave)	break;
			if (! I2C_SDA_Slave)	goto Start;
		}

		$ I2C_SDA_Slave		Out, Low;
		.wait1	I2C_SCL_Slave

Send:	count	=	8;
		data	=	~ *addr; // What the hell is going on here.
		addr$0++;

		do
		{
			.wait0	I2C_SCL_Slave
			sl		data;
		#if	_SYS(OP:SWAPC IO.N)
			swapc	_PXC(I2C_SDA_Slave);
		#else
			if (!CF)	$ I2C_SDA_Slave	In;
			if (CF)		$ I2C_SDA_Slave	Out;
		#endif
			$ I2C_SDA_Slave	Low;
			.wait1	I2C_SCL_Slave
		} while (--count);

		.wait0	I2C_SCL_Slave
		$ I2C_SDA_Slave		In;

		.wait1	I2C_SCL_Slave

Watch:	if (I2C_SDA_Slave)	goto Stop;
		if (I2C_SCL_Slave)	goto Watch;
		goto Send;
	}
	//	Write
	while (1)
	{
		if (! I2C_SCL_Slave)	break;
		if (I2C_SDA_Slave)		goto Stop;
	}

	$ I2C_SDA_Slave		Out, Low;
	.wait1	I2C_SCL_Slave

	addr$1.0	=	1;
	while (1)
	{
		.wait0	I2C_SCL_Slave
		$ I2C_SDA_Slave		In;

		count	=	8;

		do
		{
			.wait1	I2C_SCL_Slave
			if (I2C_SDA_Slave)
			{
				CF	=	1;
				slc	data;
				while (1)
				{
					if (! I2C_SCL_Slave)	break;
					if (! I2C_SDA_Slave)	goto Start;
				}
			}
			else
			{
				sl	data;
				while (1)
				{
					if (! I2C_SCL_Slave)	break;
					if (I2C_SDA_Slave)		goto Stop;
				}
			}
		} while (--count);

		$ I2C_SDA_Slave		Out, Low;
		.wait1	I2C_SCL_Slave
		if (addr$1.0)		addr	=	data;
		else
		{
			*addr	=	data;
			addr$0++;
		}
	}
}

