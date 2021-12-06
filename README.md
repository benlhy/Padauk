# Padauk Microcontrollers
This repo contains code relating to the development of Padauk Microcontrollers. 

See the original blog post here: https://westsideelectronics.com/padauk-microcontrollers/

# Introduction
These are low cost microcontrollers that cost < $0.13 per micro. This makes them incredibly cheap general purpose microcontrollers that can be used at scale - if you know what you want to do with them. Unfortunately the documentation is quite limited and spotty. However, it can be very rewarding to pick one up and embed it into a simple project

# Additional Information
The Padauk microcontrollers use a special version of C called mini-C that is just used for programming the microcontroller. Think of it as a limited dialect of C. If you don't mind some of its quirks, you can get access to the emulator as well as the programmer through LCSC: https://westsideelectronics.com/padauk-microcontrollers/

# SDCC
Small Device C Compiler supports the Padauk lineup. It uses the modern version of C and you are not subjected to the quirks of mini-C. However, you'll need to build your own programmer and will not have access to the Padauk emulator.
