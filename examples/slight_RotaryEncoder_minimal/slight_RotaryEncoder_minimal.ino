/******************************************

    slight_RotaryEncoder__minimal
        minimal example for slight_RotaryEncoder lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any type.
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

#include <slight_RotaryEncoder.h>


// ------------------------------------------
// slight_RotaryEncoder things

void myencoder_event(slight_RotaryEncoder *instance, byte event) {
    // react on event
    switch (event) {
        case slight_RotaryEncoder::event_Rotated : {
            // get current data
            int16_t temp_steps = (*instance).getSteps();
            int16_t temp_stepsAccel = (*instance).getStepsAccelerated();
            // clear data
            (*instance).clearSteps();

            Serial.print(F("  steps: "));
            Serial.println(temp_steps);
            Serial.print(F("  steps accelerated: "));
            Serial.println(temp_stepsAccel);

        } break;
        // currently there are no other events fired.
    } //end switch
}

slight_RotaryEncoder myencoder1(
    // uint8_t id_new,
    1,
    // uint8_t pin_A_new,
    A4,
    // uint8_t pin_B_new,
    A5,
    // uint8_t pulse_per_step_new,
    2,
    // tcbfOnEvent cbfCallbackOnEvent_New
    myencoder_event
);

void myencoder1_pin_changed_ISR() {
    myencoder1.updateClassic();
    // myencoder1.updateGray();
}

void myencoder_setup(Print &out) {
    out.println(F("setup slight_RotaryEncoder:")); {
        out.println(F("  pinMode INPUT_PULLUP"));
        pinMode(myencoder1.pin_A, INPUT_PULLUP);
        pinMode(myencoder1.pin_B, INPUT_PULLUP);
        out.println(F("  attach interrupts"));
        attachInterrupt(
            digitalPinToInterrupt(myencoder1.pin_A),
            myencoder1_pin_changed_ISR,
            CHANGE);
        attachInterrupt(
            digitalPinToInterrupt(myencoder1.pin_B),
            myencoder1_pin_changed_ISR,
            CHANGE);
        out.println(F("  myencoder1.begin()"));
        myencoder1.begin();
    }
    out.println(F("  finished."));
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
    Serial.println(F("slight_RotaryEncoder__minimal.ino sketch."));
    Serial.println(F("minimal example for library usage."));

    // ------------------------------------------
    myencoder_setup(Serial);

    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myencoder1.update();
    // nothing else to do here...
}

// ------------------------------------------
// THE END :-)
// ------------------------------------------
