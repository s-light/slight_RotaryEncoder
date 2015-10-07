/**************************************************************************************************
	slight_RotaryEncoder Library
		for more information changelog / history see slight_RotaryEncoder.cpp
	written by stefan krueger (s-light),
		stefan@s-light.eu, http://s-light.eu, https://github.com/s-light/
**************************************************************************************************/
/**************************************************************************************************
	license
	CC BY SA
		This work is licensed under the
		Creative Commons Attribution-ShareAlike 3.0 Unported License.
		To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.
		
	Apache License Version 2.0
		Copyright 2014 Stefan Krueger
		
		Licensed under the Apache License, Version 2.0 (the "License");
		you may not use this file except in compliance with the License.
		You may obtain a copy of the License at
		
		http://www.apache.org/licenses/LICENSE-2.0
		
		Unless required by applicable law or agreed to in writing, software
		distributed under the License is distributed on an "AS IS" BASIS,
		WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
		See the License for the specific language governing permissions and
		limitations under the License.
		
	The MIT License (MIT)
		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"),
		to deal in the Software without restriction, including without limitation
		the rights to use, copy, modify, merge, publish, distribute, sublicense,
		and/or sell copies of the Software, and to permit persons to whom the Software
		is furnished to do so, subject to the following conditions:
		The above copyright notice and this permission notice shall be included in all
		copies or substantial portions of the Software.
		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
		INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
		PARTICULAR PURPOSE AND NONINFRINGEMENT.
		IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
		OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
		http://opensource.org/licenses/mit-license.php
	
**************************************************************************************************/

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
				//typedef void (* tCbfuncValueChanged) (byte bID, int iValue, byte bAcceleration);
				//typedef void (* tCbfuncValueChanged) (slight_RotaryEncoder *pInstance, int iValue, byte bAcceleration);
				typedef void (* tcbfOnEvent) (slight_RotaryEncoder *pInstance, byte bEvent);
				
				// init
				static const byte event_NoEvent			=  0;
				
				// State
				static const byte event_StateChanged	= 10;
				
				static const byte state_NotValid		= 11;
				static const byte state_UNDEFINED		= 12;
				static const byte state_CW				= 13;
				static const byte state_CCW				= 14;
				static const byte state_HalfStep		= 15;
				
				// Value rotate
				static const byte event_Rotated			= 20;
				static const byte event_Rotated_CW		= 21;
				static const byte event_Rotated_CCW		= 22;
				
				
			// public methods
				//Constructor
				slight_RotaryEncoder(
					byte cbID_New,
					byte cbPin_A_New,
					byte cbPin_B_New,
					byte cbPulsPerStep_New,
					//tCbfuncValueChanged cbfuncValueChanged_New
					tcbfOnEvent cbfCallbackOnEvent_New
				);
				// add option 'bool inverse_logic = false'
				// --> the = false means a standard value / makes it optional?
				
				//Destructor
				~slight_RotaryEncoder();
				
				void begin();
				
				// check if class is ready to operate.
				boolean isReady();
				
				// getID
				byte getID();
				
				// state
				byte getState();
				byte printState(Print &pOut, byte bStateTemp);
				byte printState(Print &pOut);
				
				// event
				byte getLastEvent();
				byte printEvent(Print &pOut, byte bEventTemp);
				byte printEventLast(Print &pOut);
			
				
				// run every loop to update event system
				void update();
				
				// updateInput
				void updateClassic();
				
				// old
				void updateGray();
				
				
				
				// get steps
				int getSteps();
				int getStepsAccelerated();
				// get AccelerationFactor
				byte getAccelerationFactor();
				// call after you have read all data
				void resetData();
				
				
				
				
		private:
			// per object data
				
				// ID
				const byte cbID;
				
				// flag to check if the begin function is already called and the class is ready to work.
				boolean bReady;
				
				// internal state
				byte bState;
				
				// event
				byte bEvent;
				byte bEventLast;
				const tcbfOnEvent cbfCallbackOnEvent;
				
				//call back functions:
				//const tCbfuncValueChanged cbfuncValueChanged;
				
				// pins
				const byte cbPin_A;
				const byte cbPin_B;
				
				// pulses per step
				const byte cbPulsPerStep;
				
				// internal pulse counter
				byte bPulsCount;
				
				// internal Step Counter
				int iStepCount;
				
				
				// temp GrayCode store
				byte bGrayCode;
				
				static const int8_t iGrayTable[];
				
				// Acceleration calculation
				unsigned long ulTimeStamp;
				word wDuration;
				word wAcceleration_DurationStepSum;
				byte bAccelerationDuration;
				byte bAccelerationFactor;
				
				
				// is this used?
				// temp ? 
				boolean bA;
				boolean bB;
				boolean bA_last;
				boolean bB_last;
				boolean bLastFullStep;
				
			// private methods
				void calcAcceleration();
				
				void stateChange();
				
				void generateEvent(byte bEventNew);
				
				byte multiMap(byte val, byte* _in, byte* _out, uint8_t size);
	};
	
#endif //ifndef slight_RotaryEncoder_h

/** the end **/