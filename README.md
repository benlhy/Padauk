# Padauk Microcontrollers

This repo contains code relating to the development of Padauk Microcontrollers.

See the original blog post here: https://westsideelectronics.com/padauk-microcontrollers/

See the encoder build here: https://westsideelectronics.com/using-the-3-cent-microcontroller/

See the motor controller build here (featured on Hackaday!): https://westsideelectronics.com/repurposing-the-encoder-counter/

# Repo contents

1. Datasheets - datasheets for the Padauk devices (Emulator/Programmer/IC)
2. Example - Snippets of code
3. Encoder - This project converts the PFS173 into an encoder reader and exposes the count over a count/dir pin protocol and a SPI protocol.
4. Motor Controller - This project converts the PFS173 into a motor controller with position control loop and the ability to control it over SPI

# Building

You will need the programmer to be connected.

1. Download & install the latest version of the Padauk IDE here: http://www.padauk.com.tw/en/technical/index.aspx?kind=26
2. Open the project in the IDE and hit: `Execute > Build`
3. Once the compiler completes, open the interface to the programmer `Execute > Writer`

![Padauk Programmer screen](/Images/writer_page.jpg)

4. In the programmer window it will already have auto-selected the correct project. Hit `Load File` to load it to the programmer.
5. Once the loading completes, either press the Program Button on the programmer or hit the `Auto Program` button to load the binary to the microcontroller.

Do note that the programmer automatically detects the type of IC that you are going to program and provides the jumper and positioning info accordingly under "Check Jump" in the output screen.

For example, because I am using a SOP-16 IC, that corresponds to S16. If I open the back of the programmer, the jumpers should be on the row of pins labelled as JP2. I also have to insert my IC flush to the top of the ZIF socket. If I am using SOC-14 ICs, I will need to move the IC one space down

# Introduction to Padauk Microcontrollers

These are low cost microcontrollers that cost < $0.13 per micro. This makes them incredibly cheap general purpose microcontrollers that can be used at scale - if you know what you want to do with them. Unfortunately the documentation is quite limited and spotty. However, it can be very rewarding to pick one up and embed it into a simple project.

# Additional Information

The Padauk microcontrollers use a special version of C called mini-C that is just used for programming the microcontroller. Think of it as a limited dialect of C. If you don't mind some of its quirks, you can get access to the emulator as well as the programmer through LCSC: https://westsideelectronics.com/padauk-microcontrollers/

Availability can be found on the Padauk website: http://www.padauk.com.tw/en/contact/method.aspx?num=1

# SDCC

Small Device C Compiler supports the Padauk lineup. It uses the modern version of C and you are not subjected to the quirks of mini-C. However, you'll need to build your own programmer and will not have access to the Padauk emulator as well as debugger.
