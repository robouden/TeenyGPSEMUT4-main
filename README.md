# TeenyGPSEMUT4

Initial commit for GPS Emulator running on Teensy 4.1.
I also have a version for Feather_M0_Basic and Feather_M0_AdaLogger
and can port to other boards.  Only requirement is that the board has
a real-time-clock.  The SD card optional if you want to add your own routes.

Teensy 4.1 (T41) implementation:
The T41 requires 5v usb power so it must be powered independently from the Zen board.
Needs a three wire harness, Tx/Rx/Gnd, to connect to the Zen serial interface.  These three
pins are conveniently located next to each other on one corner of the T41 board.

Once powered and connected, it acts as a GPS device with 38400 baud rate and autoPVT disabled.
The Zen would then setup the emulator as it would the gps module and enable autoPVT.
The first 10 PVT packets sent are cold packets (no location, date, or time).
After 10 seconds you will be following a 9 min 23 sec infinite loop somewhere in New York.


![TeenyGPSEMUT4_Hookup](https://github.com/BeakeS/TeenyGPSEMUT4/assets/27782001/8269bbb8-12a9-469a-b85d-909273d7b444)
