## PCB files and Arduino sketches for FrSky SBUS control of Joe's Drive

Bulk of original code thanks to Patrick Ryan. This code will utilise an ESP32 to control a BB8 over RC using SBus.

### Required Libraries
* https://www.arduino.cc/reference/en/libraries/dfrobotdfplayermini/
* https://www.arduino.cc/reference/en/libraries/bolder-flight-systems-sbus/
* https://www.arduino.cc/reference/en/libraries/adafruit-bno055/
* https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
* https://www.arduino.cc/reference/en/libraries/bts7960/

### Usage

Rename or copy the drive/constants.h-default to drive/constants.h

This file contains all the tweakable settings for the droid, including PWM values, channels to use, and pins on the esp32. Update the values to match how you have your RC system set up.



### TODO
* Calibration functions
* General cleanup
