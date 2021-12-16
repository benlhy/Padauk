
#include	"extern.h"

// v1: 7 Dec 2021


// Chip used: PFS173


/*

Introduction

	There are two modes:
		1. SPI
		2. 2 Pin (count and dir)

	SPI
		0x01: reset
		0x03: read - long byte will be sent MSB first

	Count & Dir
		count: will pulse on count increment or decrement
		dir: will be high or low depending on positive or negative change


Todo
1. Implement Chip Select
2. Test SPI


*/

// PIN defines
SPI_Out			BIT	PB.2;
SPI_OutMode		BIT	PBC.2;
SPI_In			BIT	PB.3;
SPI_InMode		BIT	PBC.3;
SPI_Clk			BIT	PB.4;
SPI_ClkMode		BIT	PBC.4;


// Debug
// #define DEBUG
LED				BIT PB.7;
LED_OutMode		BIT PBC.7;

// Set this to high to let the micro know that there was a change
INTB			BIT PB.6;
INTB_OutMode	BIT PBC.6;

// Pulse this pin to let the micro know that a count was made
COUNT			BIT PB.5;
COUNT_OutMode	BIT PBC.5;

// Set this pin to let the micro know if the count was positive or negative
DIR				BIT PA.4;
DIR_OutMode		BIT PAC.4;

// PWM pin
PWM				BIT PA.3;
PWM_OutMode		Bit PAC.3;


// Switch 
BTN				BIT PA.6;
BTN_OutMode		BIT PAC.6;


// Let PA0 and PB0 be connected to the encoder, with PA0 & PB0
// configured as the interrupt pins

// Global variables

static	BYTE	SPI_Data_In;	 // stores the data coming in
WORD    counts;			      	 // stores the encoder counts
BYTE   trig;            	     // flag to keep track of the trigger
BYTE	PWM_val;				//PWM value
BYTE	curr_PWM_val;

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4	 //	SYSCLK=IHRC/4
	WORD current_value;
	BYTE save_PWM;
	BYTE BTN_flag;
	counts = 0x7FFF;			 // set it to the middle value so that we can increment up and down
	trig = 0;
	PWM_val = 127;
	curr_PWM_val = PWM_val;
	save_PWM = PWM_val;

	// Port A setup

	PADIER = 0b_0100_0001;  // Port A Digital Input Enable Register, 1/0:
                       	    // enable/disable, Bit 2:1 is reserved

	PAPH = 0b_0100_0001;	// Port A pull high register

	// Port B setup

	PBDIER = 0b_0000_0001;  // Port B Digital Input Enable Register, 1/0:
							// enable/disable

	PBPH = 0b_0000_0001;	// Port B pull high register



	// Timer 2 setup
	TM2C = 0b_0001_10_1_0;	// CLK, PA3, PWM mode, disable polarity inversion
	TM2B = 0b_0111_1111;	// count+1/256 * 100 = %
	TM2S = 0b_0_01_00000;	// 8 bit PWM, /4 prescalar 



	// Set up interrupts

	INTEN = 0b_0000_0011;   // Enable the PA0 & PB0 interrupt
	INTEGS = 0b_0000_1010;  // Edge select register - PA0 & PB0 falling edge to trigger
	INTRQ = 0;              // Clear the INTRQ register
	ENGINT;                 // Enable global interrupts

	// Set Pinmode

	set1 LED_OutMode; 		// Set LED to output based on the PAC reg
	set1 INTB_OutMode;
	set1 COUNT_OutMode;
	set1 DIR_OutMode;
	set1 PWM_OutMode;
	set0 BTN_OutMode;		// Set BTN to input 

	while (1)
	{
		if (current_value > counts) {

			#ifdef DEBUG
			// Blink once
				set1 LED;
				.delay 100000; 	// .delay is in uS, need to multiply by sysclk Mhz
				set0 LED;
			#endif

			// countmode
			set0 DIR;
			set1 COUNT;
			.delay 100000*4; 	// .delay is in uS, need to multiply by sysclk Mhz
			set0 COUNT;

			if (PWM_val < 30) {
				PWM_val = 0;
				TM2C = 0b_0001_00_1_0;	// CLK, disabled, PWM mode, disable polarity inversion
			} else {
				//TM2C = 0b_0001_10_1_0;	// CLK, PA3, PWM mode, disable polarity inversion
				PWM_val = PWM_val - 25; 
			}
			
			//TM2B = PWM_val;

			current_value = counts;
		} 

		if (current_value < counts) {
			#ifdef DEBUG
			// blink 3 times
				.REPEAT 3
					set1 LED
					.delay 100000; 	// .delay is in uS, need to multiply by sysclk Mhz
					set0 LED;
					.delay 100000; 	// .delay is in uS, need to multiply by sysclk Mhz
				.ENDM
			#endif
			// countmode
			set1 DIR;
			set1 COUNT;
			.delay 100000*4; 	// .delay is in uS, need to multiply by sysclk Mhz
			set0 COUNT;

			if (PWM_val > 225) {
				PWM_val = 255;
			} else {
				TM2C = 0b_0001_10_1_0;	// CLK, PA3, PWM mode, disable polarity inversion
				PWM_val = PWM_val + 25; 
			}
			
			//TM2B = PWM_val;
			current_value = counts; 
		}

		// setting the actual PWM value

		if (curr_PWM_val<PWM_val) {
			if (curr_PWM_val == 0) {
				TM2C = 0b_0001_10_1_0;	// CLK, PA3, PWM mode, disable polarity inversion
			}
			curr_PWM_val = curr_PWM_val + 1;
			TM2B = curr_PWM_val;
			
			//.delay 100000;
		}
		if (curr_PWM_val > PWM_val) {
			curr_PWM_val = curr_PWM_val - 1;
			TM2B = curr_PWM_val;
			if (curr_PWM_val == 0) {
				TM2C = 0b_0001_00_1_0;	// CLK, disabled, PWM mode, disable polarity inversion
			}
			//.delay 100000;
		}

		// if the button is pressed, set a flag to indicate that the button was pressed
		if (BTN == 0) {
			BTN_flag = 1;
			.delay 100000; // debounce delay
		}

		if ((BTN == 1) && (BTN_flag==1)) {
			// toggle the PWM values on release
			// if it is not zero, save the PWM value and set it to zero
			// else if it is zero set the PWM value to the value that we saved it
			if (PWM_val > 0) {
				save_PWM = PWM_val; // save the PWM value
				PWM_val = 0; 		// set it to zero

			} else  {
				PWM_val = save_PWM; // set the PWM value to the one we saved before
				
			} 
			BTN_flag = 0 ; // unset the flag
			.delay 100000; // debounce delay
			
		}
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
	set0	SPI_ClkMode;
	set0	SPI_InMode;
	set1	SPI_OutMode;

	while (1)
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
		// ignore all other commands 

	}
}

