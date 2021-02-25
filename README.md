# HC-SR04-XMEGA
HC-SR04 Library for atxmega

Cuts of at ranges longer than 1.5m

Currently only supports 32MHz

trigger = PA0
echo    = PB0

Include header file

init_us()

long int getDistance() //Returns millimeters
