/******************************************

    slight_RotaryEncoder__test
        minimal example for slight_RotaryEncoder lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any typ.
        Tested on ItsyBitsy M4
        A4 --> encoder pin A
        A5 --> encoder pin B

    libraries used:
        ~ slight_RotaryEncoder

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************/
/******************************************
CC BY SA
This work is licensed under the
Creative Commons Attribution-ShareAlike 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.


The MIT License (MIT)

Copyright (c) 2019 Stefan Krüger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
https://opensource.org/licenses/mit-license.php
******************************************/

#include <slight_RotaryEncoder_CallbackHelper.h>
#include <slight_RotaryEncoder.h>

#include "myinput.h"

// MyInput myinput = MyInput(Serial);
MyInput myinput = MyInput();

void myinput_setup(Stream &out) {
    // this is only possible here in the main..
    pinMode(myinput.myencoder.pin_A, INPUT_PULLUP);
    pinMode(myinput.myencoder.pin_B, INPUT_PULLUP);
    attachInterrupt(
        digitalPinToInterrupt(myinput.myencoder.pin_A),
        myinput_myencoder_pin_changed_ISR,
        CHANGE);
    attachInterrupt(
        digitalPinToInterrupt(myinput.myencoder.pin_B),
        myinput_myencoder_pin_changed_ISR,
        CHANGE);

    myinput.begin(out);
}

void myinput_myencoder_pin_changed_ISR() {
    myinput.myencoder.updateClassic();
    // myinput.myencoder.updateGray();
}



// ------------------------------------------
// setup
// ------------------------------------------
void setup() {
    // ------------------------------------------
    // init serial
    // wait for arduino IDE to release all serial ports after upload.
    delay(1000);
    Serial.begin(115200);
    delay(500);
    Serial.println();

    // ------------------------------------------
    // print short welcome text
    Serial.println(F("slight_RotaryEncoder__advanced2.ino sketch."));
    Serial.println(F("advanced example for library usage."));

    // ------------------------------------------
    myinput_setup(Serial);


    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myinput.update();
    // nothing else to do here...
}


// ------------------------------------------
// THE END :-)
// ------------------------------------------
