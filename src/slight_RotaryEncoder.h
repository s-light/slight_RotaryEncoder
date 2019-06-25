/******************************************
    slight_RotaryEncoder Library
    for more information see slight_RotaryEncoder.cpp
    written by stefan krueger (s-light),
    gitn@s-light.eu, http://s-light.eu, https://github.com/s-light/
*******************************************/

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

//Check if this class is allready there or if it must be created...
#ifndef slight_RotaryEncoder_Mod_h
#define slight_RotaryEncoder_Mod_h

/**
  * Includes Core Arduino functionality
  **/
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

/** Class Definition: **/
class slight_RotaryEncoder {
    public:
        // public definitions:

        //call back function definition
        //typedef void (* tCbfuncValueChanged) (uint8_t bID, int16_t iValue, uint8_t raw_Acceleration);
        //typedef void (* tCbfuncValueChanged) (slight_RotaryEncoder *pInstance, int16_t iValue, uint8_t raw_Acceleration);
        typedef void (* tcbfOnEvent) (slight_RotaryEncoder *pInstance, uint8_t event);

        // init
        static const uint8_t event_NoEvent			=  0;

        // State
        static const uint8_t event_StateChanged	= 10;

        static const uint8_t state_NotValid		= 11;
        static const uint8_t state_UNDEFINED		= 12;
        static const uint8_t state_CW				= 13;
        static const uint8_t state_CCW				= 14;
        static const uint8_t state_HalfStep		= 15;

        // Value rotate
        static const uint8_t event_Rotated			= 20;
        static const uint8_t event_Rotated_CW		= 21;
        static const uint8_t event_Rotated_CCW		= 22;


        // public methods
        slight_RotaryEncoder(
            uint8_t id_new,
            uint8_t pin_A_new,
            uint8_t pin_B_new,
            uint8_t pulse_per_step_new,
            //tCbfuncValueChanged cbfuncValueChanged_New
            tcbfOnEvent cbfCallbackOnEvent_New
        );
        ~slight_RotaryEncoder();

        void begin();
        boolean isReady();

        uint8_t getID();

        // state
        uint8_t getState();
        uint8_t printState(Print &out, uint8_t stateTemp);
        uint8_t printState(Print &out);

        // event
        uint8_t getLastEvent();
        uint8_t printEvent(Print &out, uint8_t eventTemp);
        uint8_t printevent_last(Print &out);

        // run every loop to update event system
        void update();

        // updateInput
        void updateClassic();
        // old
        void updateGray();

        // get steps
        int16_t getSteps();
        int16_t getStepsAccelerated();
        // get AccelerationFactor
        uint8_t getAccelerationFactor();
        // call after you have read all data
        void resetData();

    private:
        // per object data

        // ID
        const uint8_t id;

        // flag to check if the begin function is already called and the class is ready to work.
        boolean ready;

        // internal state
        uint8_t state;

        // event
        uint8_t event;
        uint8_t event_last;
        const tcbfOnEvent cbfCallbackOnEvent;

        //call back functions:
        //const tCbfuncValueChanged cbfuncValueChanged;

        // pins
        const uint8_t pin_A;
        const uint8_t pin_B;

        // pulses per step
        const uint8_t pulse_per_step;

        // internal pulse counter
        uint8_t pulse_count;

        // internal Step Counter
        int16_t step_count;


        // temp GrayCode store
        uint8_t gray_code;

        static const int8_t gray_table[];

        // Acceleration calculation
        uint32_t timestamp;
        uint16_t duration;
        uint16_t acceleration_duration_step_sum;
        uint8_t acceleration_duration;
        uint8_t acceleration_factor;


        // is this used?
        // temp ?
        boolean raw_A;
        boolean raw_B;
        boolean raw_A_last;
        boolean raw_B_last;
        boolean last_full_step;

        // private methods
        void calcAcceleration();

        void stateChange();

        void generateEvent(uint8_t eventNew);

        uint8_t multiMap(uint8_t val, uint8_t* _in, uint8_t* _out, uint8_t size);
};

#endif // ifndef slight_RotaryEncoder_h

/** the end **/
