// NOLINT(legal/copyright)
/******************************************************************************

    some description...

    libraries used:
        ~ slight_RotaryEncoder
            written by stefan krueger (s-light),
                git@s-light.eu, http://s-light.eu, https://github.com/s-light/
            license: MIT

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
/******************************************************************************
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
******************************************************************************/



// include Core Arduino functionality
#include <Arduino.h>

#include <slight_RotaryEncoder.h>

// include own headerfile
// NOLINTNEXTLINE(build/include)
#include "./myinput.h"

// this polluts the global namespace..
// using namespace std::placeholders;  // for `_1`
// so we use std::placeholders::_1

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions

// MyInput::MyInput(Stream &out): out(out) {
MyInput::MyInput() {
    ready = false;
}

MyInput::~MyInput() {
    end();
}

void MyInput::begin(Stream &out, slight_RotaryEncoder::tCallbackFunctionISR funcISR) {
    // clean up..
    end();
    // start up...
    if (ready == false) {
        // setup
        out.println("MyInput begin:");

        out.println("  myencoder.begin");
        myencoder.begin(funcISR);

        out.println("done:");
        // enable
        ready = true;
    }
}

void MyInput::end() {
    if (ready) {
        // nothing to do..
    }
}

void MyInput::update() {
    if (ready) {
        myencoder.update();
        if (counter != counter_last) {
            counter_last = counter;
            Serial.print("counter changed: ");
            Serial.print(counter);
            Serial.println();
        }
    }
}



// ------------------------------------------
// slight_RotaryEncoder things

void MyInput::myencoder_event(slight_RotaryEncoder *instance) {
    Serial.print(F("instance:"));
    Serial.print((*instance).id);
    Serial.print(F(" - event: "));
    (*instance).printEventLast(Serial);
    Serial.println();

    // react on event
    switch ((*instance).getEventLast()) {
        case slight_RotaryEncoder::event_StateChanged : {
            Serial.print(F("\t state: "));
            (*instance).printState(Serial);
            Serial.println();
        } break;
        // rotation
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
            counter += temp_stepsAccel;
        } break;
        // currently there are no other events fired.
    }  // end switch
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
