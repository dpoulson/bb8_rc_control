## PCB files and Arduino sketches for FrSky SBUS control of Joe's Drive

Bulk of original code thanks to Patrick Ryan. This code will utilise an ESP32 to control a BB8 over RC using SBus.

### Required Libraries
* https://www.arduino.cc/reference/en/libraries/dfrobotdfplayermini/
* https://www.arduino.cc/reference/en/libraries/bolder-flight-systems-sbus/
* https://www.arduino.cc/reference/en/libraries/adafruit-bno055/
* https://www.arduino.cc/reference/en/libraries/esp32servo/
* https://www.arduino.cc/reference/en/libraries/pid/
* https://www.arduinolibraries.info/libraries/s-port-sensor-library-for-fr-sky
* https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
* https://github.com/adafruit/Adafruit_NeoPixel
* https://www.arduino.cc/reference/en/libraries/bts7960/

### Usage

Rename or copy the drive/constants.h-default to drive/constants.h

This file contains all the tweakable settings for the droid, including PWM values, channels to use, and pins on the esp32. Update the values to match how you have your RC system set up.

### Hardware

* ESP32 - 30 pin dev board
* SBus Receiver - Archer M+
* DFPlayer Mini
* BNO055 IMU

### Discord

We have an active Droidbuilders Discord community. Feel free to ask me questions there: 

https://discord.gg/6NcK5T6dVm

### Boards

I'll be making boards available (at cost, no profit) either as a bare board or a full kit. Just add an ESP32, dfplayer, sbus receiver, and IMU. 

https://we-make-things.co.uk/product/343/

### TODO
* Calibration functions
* General cleanup
