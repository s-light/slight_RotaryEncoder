/******************************************

    slight_RotaryEncoder__minimal
        testing example for slight_RotaryEncoder lib usage.
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// definitions (global)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug Output

boolean infoled_state = 0;
const byte infoled_pin = 13;

uint32_t debugOut_LastAction = 0;
const uint16_t debugOut_interval = 1000; //ms

boolean debugOut_Serial_Enabled = 1;
boolean debugOut_LED_Enabled = 1;

// ------------------------------------------
// slight_RotaryEncoder things

void myencoder_event(slight_RotaryEncoder *instance, byte event) {
    Serial.print(F("Instance ID:"));
    Serial.println((*instance).id);

    Serial.print(F("Event: "));
    (*instance).printEvent(Serial, event);
    Serial.println();

    // react on event
    switch (event) {
        case slight_RotaryEncoder::event_StateChanged : {
            Serial.print(F("\t state: "));
            (*instance).printState(Serial);
            Serial.println();
        } break;
        // rotation
        case slight_RotaryEncoder::event_Rotated : {
            // get current data
            int16_t temp_Steps = (*instance).getSteps();
            // clear data
            (*instance).clearSteps();

            Serial.print(F("  steps: "));
            Serial.println(temp_Steps);

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// debug things

const uint8_t pin_Button = A3;
boolean flag_button_pin_changed = false;

void button_pin_changed_ISR() {
    Serial.print(F("Button!!"));
}


void debugOut_update() {
    uint32_t duration_temp = millis() - debugOut_LastAction;
    if (duration_temp > debugOut_interval) {
        debugOut_LastAction = millis();

        if ( debugOut_Serial_Enabled ) {
            Serial.print(millis());
            Serial.print(F("ms;"));
            Serial.println();
        }

        if ( debugOut_LED_Enabled ) {
            infoled_state = ! infoled_state;
            if (infoled_state) {
                //set LED to HIGH
                digitalWrite(infoled_pin, HIGH);
            } else {
                //set LED to LOW
                digitalWrite(infoled_pin, LOW);
            }
        }
    }
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

    // testing things
    Serial.println(F("  attach interrupt for button;"));
    pinMode(pin_Button, INPUT_PULLUP);
    attachInterrupt(
        digitalPinToInterrupt(pin_Button), button_pin_changed_ISR, CHANGE);
    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myencoder1.update();
    debugOut_update();
    // nothing else to do here...
}


// ------------------------------------------
// THE END :-)
// ------------------------------------------
