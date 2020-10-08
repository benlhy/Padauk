#include "extern.h"

// PA0 - clock, PA3 - data (active low), PA4 - latch (active low)

BYTE in;
BYTE count;

void FPPA0 (void) {
  .ADJUST_IC SYSCLK = IHRC/16, IHRC = 16MHz, VDD = 5V;  //	SYSCLK=IHRC/2

  PA = 0b_0000_0000;      // Port A Data register
  PAC = 0b_1111_1111;     // Port A Control register, 0/1: input/output
  PAPH = 0b_0001_1000;    // Port A Pull-High Register 0/1: disable/enable
  PADIER = 0b_1111_1001;  // Port A Digital Input Enable Register, 1/0:
                          // enable/disable, Bit 2:1 is reserved

  PB = 0b_0000_0000;      // Port B Data register
  PBC = 0b_1111_1111;     // Port B Control register, 0/1: input/output
  PBPH = 0b_0000_1000;    // Port B Pull-High Register 0/1: disable/enable
  PBDIER = 0b_1111_1111;  // Port B Digital Input Enable Register, 1/0:
                          // enable/disable

  INTEN = 0b_0000_0001;   // Enable the PA0 interrupt
  INTEGS = 0b_0000_0001;  // Edge select register - PA0 rising edge to trigger
  INTRQ = 0;              // Clear the INTRQ register
  ENGINT;                 // Enable global interrupts

  while (1) {
    if (pa.4 == 0 && count == 8) {
      PB = in;
      in = 0;  // clear the byte
	  count = 0;
    }
  }
}

void Interrupt(void) {
  PUSHAF;  // Reserve the ALU and FLAG registers
  if (INTRQ.PA0) {
    INTRQ.PA0 = 0;  // clear the interrupt bit associated with PA0
    if (pa.3 == 0) {
      in = in << 1 | 0;
    } else {
      in = in << 1 | 1;
    }
    count = count + 1;
  }
  POPAF;  // Restore ALU and FLAG registers
}  // RETI (return from interrupt) is added automatically on compilation. It
   // returns the code execution to the point in the main code where it was
   // interrupted from.