
#include	"extern.h"


void I2C_Slave(void);
BYTE flip;

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2, IHRC = 16MHz,VDD=5V;		//	SYSCLK=IHRC/2

	//	Insert Initial Code
	
	flip = 0b_0000_1111; // note that the data will be flipped on output, but better to do it on the receiving system.


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



I2C_SDA_Slave		BIT	PB.5; // SDA is PB5
I2C_SCL_Slave		BIT	PB.4; // SCL is PB4

I2C_DEVICE	EQU	0xA0		//	Please modify it by your device
							//	Because only 24Cxx use the 0xA0 for device code.
DEVICE_LEN	EQU	4			//	Only need compare Bit 7 ~ Bit 4
							//	Bit 3 ~ Bit 1 is for high address
.errz	DEVICE_LEN <= 7		//	must check 1 <= DEVICE_LEN <= 7
.errz	DEVICE_LEN >= 1

void	I2C_Slave (void)
{
	$ I2C_SCL_Slave	In; // set as input?
	$ I2C_SDA_Slave	In;

	.delay	99;		//	Wait for master ready

	WORD	addr;	//
	addr$1	=	0;	//	at the sample code, only access one byte of address. // set the first two bytes of the addr 0

	

Stop:	.wait1	I2C_SCL_Slave // this is disabled in a 1 FPPA device.
		if (! I2C_SDA_Slave)	goto Stop; // if SDA is low, go to stop?

High:	if (! I2C_SCL_Slave)	goto Stop; // if SCL is low go to Stop?
		if (I2C_SDA_Slave)		goto High; // if SDA is high go to HIGH

		// The start condition is where SDA is low and SCL is high -> wait until SCL is low.

Start:	if (! I2C_SCL_Slave)	goto Chk_Ax; // So if SCL is LOW and SDA is low, you reach this step.
		if (! I2C_SDA_Slave)	goto Start; // Else if SCL is HIGH and SDA is low, go back to top, wait for SCL to be low
		goto High; // SDA is high, go to HIGH

Chk_Ax: // Check something?
		// This section is to count the address..
	BYTE	count	=	DEVICE_LEN; // set byte count to device len (4)
	A	=	I2C_DEVICE; // set A to be equal to 0xA0
	do 
	{
		sl	A; // wtf is s1.
		.wait1	I2C_SCL_Slave // ignore
		if (I2C_SDA_Slave) // if SDA is high
		{
			if (! CF) goto Stop; // ACC Status Flag Register, Carry Flag, carry out in addition or borrow in subtraction
			while (1)
			{
				if (! I2C_SCL_Slave)	break; // wait until SCL goes low
				if (! I2C_SDA_Slave)	goto Start; // if SDA goes low, go back to start 
			}
		}
		else // if SDA is low
		{
			if (CF) goto Stop;
			while (1)
			{
				if (! I2C_SCL_Slave)	break; //continue once SCL goes low.
				if (I2C_SDA_Slave)		goto Stop; //stop if SDA is high
			}
		}
	} while (--count); //decrement the count.

	if (DEVICE_LEN != 7) // if device length is not 7,
	{
		BYTE	hi_adr	=	0;
		A	=	7 - DEVICE_LEN; //  7- 3 = 4...
		do
		{
			.wait1	I2C_SCL_Slave // ignore
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

	.wait1	I2C_SCL_Slave // ignore

	BYTE	data;

	// okay now that we are done checking the address and comparing it, lets read some data.

	if (I2C_SDA_Slave) // if SDA is high
	{	//	Read
		while (1)
		{
			if (! I2C_SCL_Slave)	break; // wait for SCL to go low.
			if (! I2C_SDA_Slave)	goto Start;
		}

		$ I2C_SDA_Slave		Out, Low; // set SDA as an output, set it to low
		.wait1	I2C_SCL_Slave // ignore.

Send:	count	=	8;// 1byte...
		
		//data = flip;
		
		data	=	~ *addr; // bitwise complement of addr pointer...
		addr$0++; // increment it?

		//data = ~ *flip; // it sends bit 0 to bit 7

		// So if data is 0b_1111_0000
		// then the output is 0b_0000_1111

		do
		{
			.wait0	I2C_SCL_Slave
			sl		data;
		#if	_SYS(OP:SWAPC IO.N) // swap the nth bit of the IO port with the carry bit
			swapc	_PXC(I2C_SDA_Slave); //convert PA.0 to PAC.0 Input/OUTPUT? what is the CF flag for?
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

Watch:	if (I2C_SDA_Slave)	goto Stop; //okay master sent SDA high, stop
		if (I2C_SCL_Slave)	goto Watch; // If SCL goes HIGH, wait until it goes low again.
		goto Send; // okay SCL is LOW, continue sending.
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

