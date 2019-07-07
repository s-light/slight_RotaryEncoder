/******************************************

    slight_RotaryEncoder library
    library for Rotary Encoder handling
    Copyright 2019 Stefan Krueger

    based on infos and ideas from:
    ~ http://www.mikrocontroller.net/articles/Drehgeber
    ~ http://www.mikrocontroller.net/topic/drehgeber-auslesen
    ~ http://www.mikrocontroller.net/topic/38863
    ~ http://hacks.ayars.org/2009/12/using-quadrature-encoder-rotary-switch.html
    ~ http://playground.arduino.cc/Main/RotaryEncoders#Waveform
    ~ http://www.heise.de/ct/04/22/links/236.shtml

    with help from:
    pYro_65 ( callback function and declaration order; http://arduino.cc/forum/index.php/topic,162249.0.html )
    fritz kr. (understanding graycode; understanding timer interrupts)


    written by stefan krueger (s-light),
    git@s-light.eu, http://s-light.eu, https://github.com/s-light/


    changelog / history
    24.04.2013 08:46 created; based on SwitchPCB_Test_RotaryEncoder_v02
    24.04.2013 23:13 added multible pulses per step
    29.04.2013 20:00 changed system to return values since last reading.
    30.04.2013 12:15 added updateGray --> implemented GrayCode concept.
    06.01.2014 21:04 added event / callback on value changed
    07.01.2014 08:15 added acceleration: outputs weighted duration between steps
    07.01.2014 22:29 updated formating
    08.01.2014 08:11 added 'print_info()'
    16.03.2014 09:40 changed system to event with pInstance
    16.03.2014 15:10 added / prepared acceleration map (currently programmed fixed)
        07.10.2015 15:15 moved to git
            more changelog/history in the git commits
        25.06.2019 12:45 prepared new library format

********************************************/

/******************************************
license

------------------------------------------
Apache License Version 2.0
Copyright 2019 Stefan Krueger
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

------------------------------------------
The MIT License (MIT)

Copyright 2019 Stefan Krüger

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

// Includes Core Arduino functionality
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

// include headerfile
// NOLINTNEXTLINE(build/include)
#include "./slight_RotaryEncoder.h"

/*******************************************/
// info
/*******************************************/
#ifdef debug_slight_RotaryEncoder
void print_info(const Print &out) {
    out.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|                       ^ ^                      |"));
    out.println(F("|                      (0,0)                     |"));
    out.println(F("|                      ( _ )                     |"));
    out.println(F("|                       \" \"                      |"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("| slight_RotaryEncoder library"));
    out.println(F("|   library for Rotary Encoder handling"));
    out.println(F("|     with acceleration support"));
    out.println(F("|"));
    out.println(F("| dream on & have fun :-)"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|"));
    out.println(F("| last changed: 25.06.2019"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println();
}
#endif


// #define debug_slight_RotaryEncoder
/*
#ifdef debug_FaderLin
    Serial.println(F("************************************************************"));
    Serial.println(F("** Welcome to Rotary World - Library will initialise... **"));
    Serial.println(F("************************************************************"));
    Serial.println(F("RotaryEncoder::begin: "));
#endif
*/



/*******************************************/
// constructor
/*******************************************/
// slight_RotaryEncoder::slight_RotaryEncoder(uint8_t bPin_A_NewValue, uint8_t bPin_B_NewValue, uint8_t bpulse_per_step_NewValue, tCBF_OnStep cbfOnStep_NewValue) {
slight_RotaryEncoder::slight_RotaryEncoder(
    uint8_t id_new,
    uint8_t pin_A_new,
    uint8_t pin_B_new,
    uint8_t pulse_per_step_new,
    tCallbackFunction callbackOnEvent_New
) :
    id(id_new),
    pin_A(pin_A_new),
    pin_B(pin_B_new),
    pulse_per_step(pulse_per_step_new),
    callbackOnEvent(callbackOnEvent_New)
// NOLINTNEXTLINE(whitespace/braces)
{
    raw_A = 0;
    raw_B = 0;
    raw_A_last = 0;
    raw_B_last = 0;
    last_full_step = 0;
    pulse_count = 0;

    event = event_NoEvent;
    event_last = event_NoEvent;

    state = state_UNDEFINED;

    timestamp = 0;
    duration = 0;
    acceleration_duration_step_sum = 0;
    acceleration_factor = 1;
    step_count = 0;

    gray_code = 0;
    flag_use_classic = false;

    ready = false;
}

// Destructor
slight_RotaryEncoder::~slight_RotaryEncoder() {
  //
}


void slight_RotaryEncoder::begin(tCallbackFunctionISR func_ISR) {
    if (ready == false) {
        // &Print out = Serial;
        // print_info(out);
        // out.println(F("************************************************************"));
        // out.println(F("** Welcome to Rotary Encoder World. Library initialises.. **"));
        // out.println(F("************************************************************"));
        // out.println(F("FaderLin::begin: "));

        // out.println(F(" initialise pins"));
        pinMode(pin_A, INPUT_PULLUP);
        pinMode(pin_B, INPUT_PULLUP);

        // out.println(F(" attach interrupts"));
        // attachInterrupt only allowes for non-member functions
        // https://arduino.stackexchange.com/a/4865/13509
        // https://github.com/arduino/ArduinoCore-avr/pull/58
        // so we need to get it passed from the main calling sketch..
        attachInterrupt(
            digitalPinToInterrupt(pin_A), func_ISR, CHANGE);
        attachInterrupt(
            digitalPinToInterrupt(pin_B), func_ISR, CHANGE);

        event = event_NoEvent;
        event_last = event_NoEvent;

        ready = true;
        // out.println(F("  done."));
    }
}


/*******************************************/
/** functions                                                                                    **/
/*******************************************/

/************************************************/
/**  public                                    **/
/************************************************/

// updateGray
// void slight_RotaryEncoder::pin_changed_ISR() {
//     if (flag_use_classic) {
//         updateClassic();
//     } else {
//         updateGray();
//     }
// }

// updateGray
void slight_RotaryEncoder::updateGray() {
    if (ready == true) {
        /** dont know how to moved to global context**/
        static const int8_t gray_table[] = {
            //        //            new        old
            // res, //    index    A    B    A    B
             0,  //       0        0    0    0    0
            -1,  //       1        0    0    0    1
            +1,  //       2        0    0    1    0
             0,  //       3        0    0    1    1
            +1,  //       4        0    1    0    0
             0,  //       5        0    1    0    1
             0,  //       6        0    1    1    0
            -1,  //       7        0    1    1    1
            -1,  //       8        1    0    0    0
             0,  //       9        1    0    0    1
             0,  //      10        1    0    1    0
            +1,  //      11        1    0    1    1
             0,  //      12        1    1    0    0
            +1,  //      13        1    1    0    1
            -1,  //      14        1    1    1    0
             0,  //      15        1    1    1    1
        };
        /**/

        // move last reading to old position (bit 0, 1)
        gray_code >>= 2;

        // write new states to bit 3, 4??
        if (digitalRead(pin_A))
        gray_code |= 0x04;        // new A state to bit 4
        if (digitalRead(pin_B))
        gray_code |= 0x08;        // new B state to bit 5

        step_count = step_count + gray_table[gray_code];
    }  // ready end
}


// called from timer interupt every 1ms
void slight_RotaryEncoder::updateClassic() {
    if (ready == true) {
        /**
        rast punkt immer bei 0,0 oder 1,1
        CCW
        ---- A, B
        111: 0, 1
        111: 0, 0    !
        111: 1, 0
        111: 1, 1    !
        ---- A, B
        111: 0, 1
        111: 0, 0    !
        111: 1, 0
        111: 1, 1    !

        CW
        ---- A, B
        111: 1, 0
        111: 0, 0    !
        111: 0, 1
        111: 1, 1    !
        ---- A, B
        111: 1, 0
        111: 0, 0    !
        111: 0, 1
        111: 1, 1    !

        Error
        ---- A, B
        111: 1, 0
        111: 0, 0
        111:1
        111+1
        111: 0, 1
        111: 1, 1
        111:1
        111+1
        111: 1, 0
        111: 1, 1
        111:2        <--Wrong reading!!!
        111-1
        111: 1, 0
        111: 0, 0
        111:1
        111+1

        this reading is wrong.
        it is not alowed to have 1,1; 1,0; 1,1

        **/

        boolean bTemp_A = digitalRead(pin_A);
        boolean bTemp_B = digitalRead(pin_B);
        if ((bTemp_A != raw_A) || (bTemp_B != raw_B)) {
            // status changed.
            raw_A = bTemp_A;
            raw_B = bTemp_B;

            #ifdef debug_FaderLin
            // Serial.print(bNr);
            // Serial.print("?: ");
            // Serial.print(raw_A);
            // Serial.print(", ");
            // Serial.println(raw_B);
            #endif

            // half step; save state of encoder pins for direction check
            if (raw_A != raw_B) {
                raw_A_last = raw_A;
                raw_B_last = raw_B;
                state = state_HalfStep;
                stateChange();
            } else {
                // full step; one step is finished. -->
                // This is Only true if the last full step was not the same level (high or low)
                if ( (raw_A == raw_B) && (raw_A != last_full_step) ) {
                    // remember LastFullStep (error correction)
                    last_full_step = raw_A;
                    // Encoder turns CW
                    if (raw_A == raw_B_last)  {
                        // Do Something
                        state = state_CW;
                        stateChange();
                    } else {
                        // Encoder turns CCW
                        if (raw_B == raw_A_last)  {
                            state = state_CCW;
                            stateChange();
                        }
                    }
                }
            }
        }
    }  // ready end
}


// called from main loop
void slight_RotaryEncoder::update() {
    if (ready == true) {
        // update event system
        if ( step_count != 0 ) {
            // calc Acceleration
            calcAcceleration();
            // fire event
            generateEvent(event_Rotated);
        }
    }  // ready end
}


boolean slight_RotaryEncoder::isReady() {
    return ready;
}


uint8_t slight_RotaryEncoder::getState() {
    return state;
}

uint8_t slight_RotaryEncoder::printState(Print &out, uint8_t stateTemp) {
    switch (stateTemp) {
        case slight_RotaryEncoder::state_NotValid : {
            out.print(F("NotValid"));
        } break;
        case slight_RotaryEncoder::state_UNDEFINED : {
            out.print(F("UNDEFINED"));
        } break;
        case slight_RotaryEncoder::state_CW : {
            out.print(F("CW"));
        } break;
        case slight_RotaryEncoder::state_CCW : {
            out.print(F("CCW"));
        } break;
        case slight_RotaryEncoder::state_HalfStep : {
            out.print(F("HalfStep"));
        } break;
        default: {
            out.print(F("error: '"));
            out.print(state);
            out.print(F(" ' is not a know state."));
        }
    }  // end switch
    return stateTemp;
}

uint8_t slight_RotaryEncoder::printState(Print &out) {
    printState(out, state);
    return state;
}


uint8_t slight_RotaryEncoder::getEventLast() {
    return event_last;
}

uint8_t slight_RotaryEncoder::printEvent(Print &out, uint8_t eventTemp) {
    switch (eventTemp) {
        case slight_RotaryEncoder::event_NoEvent : {
            out.print(F("no event"));
        } break;
        case slight_RotaryEncoder::event_StateChanged : {
            out.print(F("state changed"));
        } break;
        // rotation
        case slight_RotaryEncoder::event_Rotated : {
            out.print(F("rotated"));
        } break;
        case slight_RotaryEncoder::event_Rotated_CW : {
            out.print(F("rotated CW"));
        } break;
        case slight_RotaryEncoder::event_Rotated_CCW : {
            out.print(F("rotated CCW"));
        } break;
        default: {
            out.print(F("error: '"));
            out.print(state);
            out.print(F(" ' is not a know event."));
        }
    }  // end switch
    return state;
}

uint8_t slight_RotaryEncoder::printEventLast(Print &out) {
    printEvent(out, event_last);
    return event_last;
}


int16_t slight_RotaryEncoder::getSteps() {
    return step_count;
}

int16_t slight_RotaryEncoder::getStepsAccelerated() {
    return (step_count * acceleration_factor);
}

uint8_t slight_RotaryEncoder::getAccelerationFactor() {
    return acceleration_factor;
}

void slight_RotaryEncoder::clearSteps() {
    step_count = 0;
    acceleration_factor = 0;
    acceleration_duration = 0;
    acceleration_duration_step_sum = 0;
}

/************************************************/
// privat
/************************************************/

void slight_RotaryEncoder::calcAcceleration() {
    // acceleration_factor = acceleration_duration_step_sum / abs(step_count);
    #ifdef debug_FaderLin
    Serial.print(F("update "));
    Serial.println(id);
    Serial.print(F(" acceleration_duration_step_sum ["));
    Serial.print(acceleration_duration_step_sum);
    Serial.print(F("] / abs(step_count["));
    Serial.print(step_count);
    Serial.print(F("]) : "));
    Serial.println(acceleration_duration_step_sum / abs(step_count));
    #endif

    const uint8_t accMap_cbSize = 5;
    uint8_t accMap_wIn[accMap_cbSize] =  {  0,  10,  20,  21, 255};
    uint8_t accMap_wOut[accMap_cbSize] = { 10,  5,   5,   1,   1};

    // calculate average duration
    acceleration_duration = acceleration_duration_step_sum / abs(step_count);

    // map duration to factor
    acceleration_factor =  multiMap(
        acceleration_duration, accMap_wIn, accMap_wOut, accMap_cbSize);
}

void slight_RotaryEncoder::stateChange() {
    #ifdef debug_FaderLin
    /*
    printState();
    Serial.print(" count: ");
    Serial.print(pulse_count);
    Serial.println(); **/
    #endif
    if ( (state == state_CW) || (state == state_CCW) ) {
        // One new Pulse - add to counter
        pulse_count = pulse_count + 1;
        // check if a full Step is reached
        if (pulse_count >= pulse_per_step) {
            // One Step for the User
            pulse_count = 0;
            // calc duration between last and this step.
            // ulDuration = micros() - timestamp;
            uint32_t ulDuration_Temp = millis() - timestamp;

            if (ulDuration_Temp < 50) {
                #ifdef debug_FaderLin
                Serial.println(F(" < 30"));
                #endif
                duration = ulDuration_Temp;
            } else {
                #ifdef debug_FaderLin
                Serial.println(F(" > 30"));
                #endif
                duration = 50;
            }

            // acceleration base
            acceleration_duration_step_sum = acceleration_duration_step_sum + duration;

            #ifdef debug_FaderLin
            /** DEBUG OUT **/
            Serial.print(F("stateChange "));
            Serial.println(id);
            Serial.print(F(" Duration:    "));
            Serial.println(duration);
            Serial.print(F(" DurationSum: "));
            Serial.println(acceleration_duration_step_sum);
            // Serial.print(": ");
            // printState();
            // Serial.println();/
            #endif

            // Update step_count
            switch (state) {
                case state_CW: {
                    // step_count = step_count + int( 1 * acceleration_factor );
                    step_count = step_count + 1;
                    break;
                }
                case state_CCW: {
                    // step_count = step_count - ( 1 * acceleration_factor );
                    step_count = step_count - 1;
                    break;
                }
            }

            // reset TimeStamp for next measurment
            // timestamp = micros();
            timestamp = millis();
        }
    }
    generateEvent(event_StateChanged);
}


void slight_RotaryEncoder::generateEvent(uint8_t event_new) {
    event = event_new;
    // call event
    // if (event != event_NoEvent) {
    if ((event != event_NoEvent) && (event != event_StateChanged)) {
        callbackOnEvent(this);
    }
    event_last = event;
    event = event_NoEvent;
}

// MultiMap
//   http://arduino.cc/playground/Main/MultiMap
uint8_t slight_RotaryEncoder::multiMap(uint8_t val, uint8_t* _in, uint8_t* _out, uint8_t size) {
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while (val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}


/*********************************************/
/** THE END                                                                                        **/
/*********************************************/
