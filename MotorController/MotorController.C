// Motor Encoder
// This project uses encoder counts to do position and velocity control of the motor


#include	"extern.h"

// v1: 7 Dec 2021
// v2: 21 Dec 2021 - updated for motor controller


// Chip used: PFS173


/*

Introduction

	Input modes:
		1. SPI
		
	SPI
		0x01: reset
		0x03: read - long byte will be sent MSB first
		0x05: put - put data for position

	Count & Dir
		count: will pulse on count increment or decrement
		dir: will be high or low depending on positive or negative change

	Pins
		PA3: PWM
		PA4: DIR
		PA6: BTN
		


Todo
1. Implement Chip Select
2. Test SPI


*/

// Debug
//#define DEBUG


// PIN defines
SPI_Out			BIT	PB.2;
SPI_OutMode		BIT	PBC.2;
SPI_In			BIT	PB.3;
SPI_InMode		BIT	PBC.3;
SPI_Clk			BIT	PB.4;
SPI_ClkMode		BIT	PBC.4;



LED				BIT PB.7;
LED_OutMode		BIT PBC.7;


// Direction pin for the motor
DIR				BIT PA.4;
DIR_OutMode		BIT PAC.4;

// PWM pin for the motor
PWM				BIT PA.3;
PWM_OutMode		Bit PAC.3;


// Switch to advance debug state
BTN				BIT PA.6;
BTN_OutMode		BIT PAC.6;


// Let PA0 and PB0 be connected to the encoder, with PA0 & PB0
// configured as the interrupt pins

// Global variables

static	BYTE	SPI_Data_In;	// stores the data coming in
WORD    counts;			      	// stores the encoder counts
BYTE  	trig;            	    // flag to keep track of the trigger
BYTE	PWM_val;				// PWM value
BYTE	curr_PWM_val;
static WORD		desiredPos;				// Desired position
WORD 	center;


void goDesiredPos(void); // function prototype
void	SPI_HandShake (void); // function prototype

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4	 //	SYSCLK=IHRC/4
	WORD current_value;
	BYTE save_PWM;
	BYTE BTN_flag;
	

	
	center = 0x7FFF;			// Center value
	counts = 0x7FFF;			// set it to the middle value so that we can increment up and down
	trig = 0;
	PWM_val = 0;
	curr_PWM_val = PWM_val;
	save_PWM = PWM_val;

	// Port A setup

	PADIER = 0b_0100_0001;  // Port A Digital Input Enable Register, 1/0:
                       	    // enable/disable, Bit 2:1 is reserved

	PAPH = 0b_0100_0001;	// Port A pull high register

	// Port B setup

	PBDIER = 0b_0001_1111;  // Port B Digital Input Enable Register, 1/0:
							// enable/disable

	PBPH = 0b_0000_0001;	// Port B pull high register



	// Timer 2 setup
	TM2C = 0b_0001_10_1_0;	// CLK, PA3, PWM mode, disable polarity inversion
	TM2B = 0b_0000_0000;	// count+1/256 * 100 = %
	TM2S = 0b_0_01_00000;	// 8 bit PWM, /4 prescalar 



	// Set up interrupts

	INTEN = 0b_0000_0011;   // Enable the PA0 & PB0 interrupt
	INTEGS = 0b_0000_1010;  // Edge select register - PA0 & PB0 falling edge to trigger
	INTRQ = 0;              // Clear the INTRQ register
	ENGINT;                 // Enable global interrupts

	// Set Pinmode

	set1 LED_OutMode; 		// Set LED to output based on the PAC reg
	set1 DIR_OutMode;
	set1 PWM_OutMode;
	set0 BTN_OutMode;		// Set BTN to input 
		set0	SPI_ClkMode;	// input
	set0	SPI_InMode;		// input	
	set1	SPI_OutMode;	// output


	while (1)
	{
		
		#ifdef DEBUG
			// initialize values
			desiredPos = 0xFFFF-2000 + 1 ;	// -ve value relative from current position
			counts = center;		// reset count
			goDesiredPos();
			
			TM2C = 0b_0001_00_1_0; // disable
			.wait0 BTN;
			.wait1 BTN;
			TM2C = 0b_0001_10_1_0;	// enable


			// initialize values
			desiredPos = 200;	// relative from current position
			counts = center;		// reset count
			goDesiredPos();

			TM2C = 0b_0001_00_1_0; // disable
			.wait0 BTN;
			.wait1 BTN;
			TM2C = 0b_0001_10_1_0;	// enable
		#endif

		
		SPI_Handshake();
		TM2C = 0b_0001_10_1_0;	// enable
		counts = center;
		goDesiredPos();
		TM2C = 0b_0001_00_1_0; // disable

	
	}
}


void	Interrupt (void)
{
	pushaf;


	if (Intrq.PA0)
	{
		Intrq.PA0 = 0; // if PA0 level changed, clear the interrupt
		// check if trig has been set
		if (trig == 1 && PB.0 == 0) {
			// PB0 has already set the trigger flag - i.e. it fell before us
			trig = 0 ;
			counts = counts -1; 
		} else {
			trig = 1;
		}
	}

	if (Intrq.PB0) {
		Intrq.PB0 = 0;
		// check if trig has been set
		if (trig == 1 && PA.0 == 0) {
			// PA0 has already set the trigger flag - i.e. it fell before us
			trig = 0 ;
			counts = counts + 1; 
		} else {
			trig = 1;
		}
	}

	popaf;
}

void		goDesiredPos(void) {
		WORD diff;					// Stores the error value
		WORD pos;
		WORD prevDiff;


		// desired outcome 87CF
		// - 2000 applied, F830

		pos = center - desiredPos; 
		prevDiff = 0;

		do {

			// okay because there are no negative numbers so this is hack
			// Use just P in the PID 
			
			if (counts > pos ) {
				
				diff = counts - pos + prevDiff; // calculate error = current position - desired position
				prevDiff = prevDiff + counts - pos; // increment error (diff is always positive...)
				 
				
				set1 DIR;
			} else if (counts < pos) {
				diff = pos - counts + prevDiff;
				prevDiff = prevDiff + counts - pos; // increment error (diff is always positive...)
				set0 DIR;
			}
			
			
			prevDiff = prevDiff >> 2; // devide by 
			diff = diff >> 3; // divide by 8 
			

			// limit the error

			if (diff > 150) {
				diff = 100;
			}

			if (diff <= 20 ) {
				diff = 20; // minimum
			}

			if (curr_PWM_val > diff) {
				curr_PWM_val--;
			}

			if (curr_PWM_val < diff) {
				curr_PWM_val++;
			}

			TM2B = curr_PWM_val; // set PWM
			.delay 1000*4; // artifical delay so that it doesn't go boom


		} while (counts != pos);

}

// SPI is configured in slave mode

static	void	SPI_Send (void)
{
	BYTE	cnt;
	BYTE	SPI_Data_Out;

	SPI_Data_Out	=	A;
	cnt	=	8;

	do
	{
		.wait1	SPI_Clk;      		// wait for SPI CLK to go to 1, for a single series FPPA, this is replaced by a instruction to go back
		sl		SPI_Data_Out;       // shift left 1 bit for SPI_DATA Out into C
		.wait0	SPI_Clk;			// wait for SPI CLK to go to 0
		.swapc_o SPI_Out;           // set SPI_OUT to the C register value
	} while (--cnt);
}



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

	
		SPI_Receive(); 
		// check SDI_Data_In, if 1 (01) = reset, if 3 (10) = read back
		if (SPI_Data_In == 0x01) {
			counts = 0x7FFF;
		}
		if (SPI_Data_In == 0x03) {
			A	=	counts >> 8; 	// send out the first 8 high bits
			SPI_Send(); 			// send out the first 8 bits
			A	=	counts & 0xFF; 	// set A to the last low bits
			SPI_Send(); 			// send out the next 8 bits
		}
		if (SPI_Data_In == 0x05) {
			WORD newPos;
			SPI_Receive(); 			// data is stored in...?
			//desiredPos = SPI_Data_In;
			newPos = SPI_Data_In << 8; // first 8 bits 
			SPI_Receive();
			newPos = newPos | SPI_Data_In; // next 8 bits
			desiredPos = newPos;
		}
		// ignore all other commands 
		

	
}


