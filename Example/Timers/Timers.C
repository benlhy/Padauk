
#include "extern.h"

// OUTPUT PIN: PA3, this function will toggle the PA3 pin at a set frequency
// Freq = CLOCK / (2 x (K + 1) x S1 x (S2 + 1))
// K: Bound register
// S1: Prescalar, S2: scalar

void FPPA0(void) {
  .ADJUST_IC SYSCLK=IHRC/4;  //	SYSCLK=IHRC/4

  // Compressed syntax for LEDs

  BIT LED : PA.3;
  $ LED OUT;  // set PA3 as the output, with output high

  // Examine the .inc file for the syntax

  $ TM2C SYSCLK,PA3,PERIOD; // Clock: system clock, Output: PA3, mode: Period, polarity: do not inverse
  $ TM2S 8BIT,/64,/32;
  TM2B =  0b_0001_1111; // bound value: 127 (base value to reset timer x prescalars)
  //TM2C = 0b_0000_0000;

  //	Insert Initial Code

  while (1) {
    //$ LED HIGH,LOW,HIGH;
	
    //.delay(10000);
    //$ LED LOW;
    //.delay(10000);
    //		...
    //		wdreset;
  }
}

void Interrupt(void) {
  pushaf;

  if (Intrq.T16) {  //	T16 Trig
    //	User can add code
    Intrq.T16 = 0;
    //...
  }

  popaf;
}
