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
#include "slight_RotaryEncoder.h"

/*******************************************/
// info
/*******************************************/
#ifdef debug_slight_RotaryEncoder
void print_info(Print &pOut) {
    pOut.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    pOut.println(F("|                       ^ ^                      |"));
    pOut.println(F("|                      (0,0)                     |"));
    pOut.println(F("|                      ( _ )                     |"));
    pOut.println(F("|                       \" \"                      |"));
    pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    pOut.println(F("| slight_RotaryEncoder library"));
    pOut.println(F("|   library for Rotary Encoder handling"));
    pOut.println(F("|     with acceleration support"));
    pOut.println(F("|"));
    pOut.println(F("| dream on & have fun :-)"));
    pOut.println(F("|"));
    pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    pOut.println(F("|"));
    pOut.println(F("| last changed: 25.06.2019"));
    pOut.println(F("|"));
    pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    pOut.println();
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
// slight_RotaryEncoder::slight_RotaryEncoder(byte bPin_A_NewValue, byte bPin_B_NewValue, byte bPulsPerStep_NewValue, tCBF_OnStep cbfOnStep_NewValue) {
slight_RotaryEncoder::slight_RotaryEncoder(
    byte cbID_New,
    byte cbPin_A_New,
    byte cbPin_B_New,
    byte cbPulsPerStep_New,
    // tCbfuncValueChanged cbfuncValueChanged_New
    tcbfOnEvent cbfCallbackOnEvent_New
)
:	cbID (cbID_New),
    cbPin_A ( cbPin_A_New ),
    cbPin_B ( cbPin_B_New ),
    cbPulsPerStep ( cbPulsPerStep_New ),
    //cbfuncValueChanged (cbfuncValueChanged_New)
    cbfCallbackOnEvent(cbfCallbackOnEvent_New)
{

    bA				= 0;
    bB				= 0;
    bA_last			= 0;
    bB_last			= 0;
    bLastFullStep	= 0;
    bPulsCount		= 0;

    bEvent		= event_NoEvent;
    bEventLast	= event_NoEvent;

    bState		= state_UNDEFINED;

    ulTimeStamp			= 0;
    wDuration			= 0;
    wAcceleration_DurationStepSum		= 0;
    bAccelerationFactor	= 1;
    iStepCount			= 0;

    bGrayCode			= 0;

    bReady = false;
}

// Destructor
slight_RotaryEncoder::~slight_RotaryEncoder() {
  //
}


void slight_RotaryEncoder::begin() {
    if (bReady == false) {
    #ifdef debug_FaderLin
    void print_info(Serial);
    Serial.println(F("************************************************************"));
    Serial.println(F("** Welcome to Rotary Encoder World. Library initialises.. **"));
    Serial.println(F("************************************************************"));
    Serial.println(F("FaderLin::begin: "));

    Serial.println(F("\t pinMode(cbPin_A, INPUT_PULLUP);"));
    Serial.println(F("\t pinMode(cbPin_B, INPUT_PULLUP);"));
    #endif
    // initialise pins
    pinMode(cbPin_A, INPUT_PULLUP);
    pinMode(cbPin_B, INPUT_PULLUP);

    bEvent			= event_NoEvent;
    bEventLast		= event_NoEvent;

    bReady = true;
    #ifdef debug_FaderLin
    Serial.println(F("\t bReady= true"));
    Serial.println(F("\t run."));
    #endif
    }
}


/*******************************************/
/** functions                                                                                    **/
/*******************************************/

/************************************************/
/**  public                                    **/
/************************************************/

// updateGray
void slight_RotaryEncoder::updateGray() {
    if (bReady == true) {
    /** dont know how to moved to global context**/
    const static int8_t iGrayTable[] = {
    //		//			new		old
    // res, //	index	A	B	A	B
     0, //	 0		0	0	0	0
    -1, //	 1		0	0	0	1
    +1, //	 2		0	0	1	0
     0, //	 3		0	0	1	1
    +1, //	 4		0	1	0	0
     0, //	 5		0	1	0	1
     0, //	 6		0	1	1	0
    -1, //	 7		0	1	1	1
    -1, //	 8		1	0	0	0
     0, //	 9		1	0	0	1
     0, //	10		1	0	1	0
    +1, //	11		1	0	1	1
     0, //	12		1	1	0	0
    +1, //	13		1	1	0	1
    -1, //	14		1	1	1	0
     0, //	15		1	1	1	1
    };
    /**/

    // move last reading to old position (bit 0, 1)
    bGrayCode >>= 2;

    // write new states to bit 3, 4??
    if( digitalRead(cbPin_A) )
    bGrayCode |= 0x04;		// new A state to bit 4
    if( digitalRead(cbPin_B) )
    bGrayCode |= 0x08;		// new B state to bit 5

    //
    iStepCount = iStepCount + iGrayTable[bGrayCode];
    } // ready end
}


// called from timer interupt every 1ms
void slight_RotaryEncoder::updateClassic() {
    if (bReady == true) {
    /**
    rast punkt immer bei 0,0 oder 1,1
    CCW
    ---- A, B
    111: 0, 1
    111: 0, 0	!
    111: 1, 0
    111: 1, 1	!
    ---- A, B
    111: 0, 1
    111: 0, 0	!
    111: 1, 0
    111: 1, 1	!

    CW
    ---- A, B
    111: 1, 0
    111: 0, 0	!
    111: 0, 1
    111: 1, 1	!
    ---- A, B
    111: 1, 0
    111: 0, 0	!
    111: 0, 1
    111: 1, 1	!

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
    111:2		<--Wrong reading!!!
    111-1
    111: 1, 0
    111: 0, 0
    111:1
    111+1

    this reading is wrong.
    it is not alowed to have 1,1; 1,0; 1,1

    **/

    boolean bTemp_A = digitalRead(cbPin_A);
    boolean bTemp_B = digitalRead(cbPin_B);
    if ( (bTemp_A != bA) || (bTemp_B != bB)) {
    //status changed.
    bA = bTemp_A;
    bB = bTemp_B;

    #ifdef debug_FaderLin
    /** **
    Serial.print(bNr);
    Serial.print("?: ");
    Serial.print(bA);
    Serial.print(", ");
    Serial.println(bB);
    /** **/
    #endif

    // half step; save state of encoder pins for direction check
    if (bA != bB) {
    bA_last = bA;
    bB_last = bB;
    bState = state_HalfStep;
    stateChange();
    } else {
    // full step; one step is finished. --> This is Only true if the last full step was not the same level (high or low)
    if ( (bA == bB) && (bA != bLastFullStep) ) {
    //remember LastFullStep (error correction)
    bLastFullStep = bA;
    // Encoder turns CW
    if (bA == bB_last)  {
    //Do Something
    bState = state_CW;
    stateChange();
    } else {
    // Encoder turns CCW
    if (bB == bA_last)  {
    bState = state_CCW;
    stateChange();
    }
    }
    }
    }
    }
    } // ready end
}


// called from main loop
void slight_RotaryEncoder::update() {
    if (bReady == true) {
    // update event system
    if ( iStepCount != 0 ) {
    // calc Acceleration
    calcAcceleration();
    // fire event
    generateEvent(event_Rotated);
    }
    } // ready end
}



byte slight_RotaryEncoder::getID() {
    return cbID;
}

boolean slight_RotaryEncoder::isReady() {
    return bReady;
}


byte slight_RotaryEncoder::getState() {
    return bState;
};

byte slight_RotaryEncoder::printState(Print &pOut, byte bStateTemp) {
    switch (bStateTemp) {
    case slight_RotaryEncoder::state_NotValid : {
    pOut.print(F("NotValid"));
    } break;
    case slight_RotaryEncoder::state_UNDEFINED : {
    pOut.print(F("UNDEFINED"));
    } break;
    case slight_RotaryEncoder::state_CW : {
    pOut.print(F("CW"));
    } break;
    case slight_RotaryEncoder::state_CCW : {
    pOut.print(F("CCW"));
    } break;
    case slight_RotaryEncoder::state_HalfStep : {
    pOut.print(F("HalfStep"));
    } break;
    default: {
    pOut.print(F("error: '"));
    pOut.print(bState);
    pOut.print(F(" ' is not a know state."));
    }
    } //end switch
    return bStateTemp;
};

byte slight_RotaryEncoder::printState(Print &pOut) {
    printState(pOut, bState);
    return bState;
};


byte slight_RotaryEncoder::getLastEvent() {
    return bEventLast;
};

byte slight_RotaryEncoder::printEvent(Print &pOut, byte bEventTemp) {
    switch (bEventTemp) {
    case slight_RotaryEncoder::event_NoEvent : {
    pOut.print(F("no event"));
    } break;

    case slight_RotaryEncoder::event_StateChanged : {
    pOut.print(F("state changed"));
    } break;

    // rotation
    case slight_RotaryEncoder::event_Rotated : {
    pOut.print(F("rotated"));
    } break;
    case slight_RotaryEncoder::event_Rotated_CW : {
    pOut.print(F("rotated CW"));
    } break;
    case slight_RotaryEncoder::event_Rotated_CCW : {
    pOut.print(F("rotated CCW"));
    } break;


    default: {
    pOut.print(F("error: '"));
    pOut.print(bState);
    pOut.print(F(" ' is not a know event."));
    }
    } //end switch
    return bState;
};

byte slight_RotaryEncoder::printEventLast(Print &pOut) {
    printEvent(pOut, bEventLast);
    return bEventLast;
};





int slight_RotaryEncoder::getSteps() {
    return iStepCount;
}

int slight_RotaryEncoder::getStepsAccelerated() {
    return (iStepCount * bAccelerationFactor);
}

byte slight_RotaryEncoder::getAccelerationFactor() {
    return bAccelerationFactor;
}

void slight_RotaryEncoder::resetData() {
    iStepCount						= 0;
    bAccelerationFactor				= 0;
    bAccelerationDuration			= 0;
    wAcceleration_DurationStepSum	= 0;
}

/************************************************/
/**  privat                                    **/
/************************************************/

void slight_RotaryEncoder::calcAcceleration() {
    //bAccelerationFactor = wAcceleration_DurationStepSum / abs(iStepCount);
    #ifdef debug_FaderLin
    Serial.print(F("update "));
    Serial.println(cbID);
    Serial.print(F(" wAcceleration_DurationStepSum ["));
    Serial.print(wAcceleration_DurationStepSum);
    Serial.print(F("] / abs(iStepCount["));
    Serial.print(iStepCount);
    Serial.print(F("]) : "));
    Serial.println(wAcceleration_DurationStepSum / abs(iStepCount));
    #endif

    const byte accMap_cbSize		= 6;
    byte accMap_wIn [accMap_cbSize]	= {   0,  10,  11,  25,  26, 255};
    byte accMap_wOut[accMap_cbSize]	= {  10,  10,   5,   5,   1,   1};

    // calculate average duration
    bAccelerationDuration = wAcceleration_DurationStepSum / abs(iStepCount);

    // map duration to factor
    bAccelerationFactor =  multiMap(bAccelerationDuration, accMap_wIn, accMap_wOut, accMap_cbSize);

}

void slight_RotaryEncoder::stateChange() {

    #ifdef debug_FaderLin
    /*
    printState();
    Serial.print(" count: ");
    Serial.print(bPulsCount);
    Serial.println(); **/
    #endif


    if ( (bState == state_CW) || (bState == state_CCW) ) {
    // One new Pulse - add to counter
    bPulsCount = bPulsCount + 1;

    //check if a full Step is reached
    if (bPulsCount >= cbPulsPerStep) {
    // One Step for the User
    bPulsCount = 0;
    // calc duration between last and this step.
    //ulDuration = micros() - ulTimeStamp;
    unsigned long ulDuration_Temp = millis() - ulTimeStamp;

    if (ulDuration_Temp < 50) {
    #ifdef debug_FaderLin
    Serial.println(F(" < 30"));
    #endif
    wDuration = ulDuration_Temp;
    } else {
    #ifdef debug_FaderLin
    Serial.println(F(" > 30"));
    #endif
    wDuration = 50;
    }

    // acceleration base
    wAcceleration_DurationStepSum = wAcceleration_DurationStepSum + wDuration;

    #ifdef debug_FaderLin
    /** DEBUG OUT **/
    Serial.print(F("stateChange "));
    Serial.println(cbID);
    Serial.print(F(" Duration:    "));
    Serial.println(wDuration);
    Serial.print(F(" DurationSum: "));
    Serial.println(wAcceleration_DurationStepSum);
    //Serial.print(": ");
    //printState();
    //Serial.println();/
    #endif

    // Update iStepCount
    switch (bState) {
    case state_CW: {
    //iStepCount = iStepCount + int( 1 * bAccelerationFactor );
    iStepCount = iStepCount + 1;
    break;
    }
    case state_CCW: {
    //iStepCount = iStepCount - ( 1 * bAccelerationFactor );
    iStepCount = iStepCount - 1;
    break;
    }
    }

    // reset TimeStamp for next measurment
    //ulTimeStamp = micros();
    ulTimeStamp = millis();
    }
    }


    generateEvent(event_StateChanged);


}


void slight_RotaryEncoder::generateEvent(byte bEventNew) {
    bEvent = bEventNew;
    // call event
    //if (bEvent != event_NoEvent) {
    if ( (bEvent != event_NoEvent) && (bEvent != event_StateChanged) ){
    cbfCallbackOnEvent(this, bEvent);
    }
    bEventLast = bEvent;
    bEvent = event_NoEvent;
}

// MultiMap
//   http://arduino.cc/playground/Main/MultiMap
byte slight_RotaryEncoder::multiMap(byte val, byte* _in, byte* _out, uint8_t size) {
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}


/*********************************************/
/** THE END                                                                                        **/
/*********************************************/
