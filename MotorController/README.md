# Encoder Count
1 round appears to be 355 counts.

# Control
Need to send 0x05 as the command, and then a 2 byte value for the relative position of the motor to turn to.

| Command | Value | Action |
|--------|------|------|
|0x01| NA | Reset the count to 0x7FFF |
|0x03| NA | Send the current count value as two bytes|
|0x05| uint16 | Move the motor to the position specified by (value), relative |
|0x07| uint16 | Move the motor to the position specified by (value), absolute |

Transition from Low to High on the `INTC` pin indicates that the action is complete.