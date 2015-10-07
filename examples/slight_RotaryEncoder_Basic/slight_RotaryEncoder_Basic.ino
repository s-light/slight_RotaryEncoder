/**************************************************************************************************
		
	slight_RotaryEncoder_Basic
		Basic Example for slight_RotaryEncoder library
		debugport: serial interface 115200baud
	
	hardware:
		Board:
		Tested on Arduino Leonardo compatible board
	
	libraries used:
		~ slight_RotaryEncoder
	
	
	written by stefan krueger (s-light),
		stefan@s-light.eu, http://s-light.eu, https://github.com/s-light/
	
	
	changelog / history
		08.01.2014 08:23 based on IOController_RotaryEncoder_HWTest_2.ino
		08.01.2014 08:40 cleaned up to have a basic example.
		09.01.2014 08:53 tested. workes smoothly (sometimes there are some wrong readings..)
		16.03.2014 11:42 changed to use new onEvent system
	
	
	TO DO:
		~ test on different encoders
		
	
	
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
		Copyright (c) 2014 stefan krueger
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
/**************************************************************************************************/
/** Includes:  (must be at the beginning of the file.)                                           **/
/**************************************************************************************************/
// use "" for files in same directory as .ino
//#include "file.h"

#include <slight_RotaryEncoder.h>

/**************************************************************************************************/
/** info                                                                                         **/
/**************************************************************************************************/
void print_info(Print &pOut) {
	pOut.println();
	//             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("|                       ^ ^                      |"));
	pOut.println(F("|                      (0,0)                     |"));
	pOut.println(F("|                      ( _ )                     |"));
	pOut.println(F("|                       \" \"                      |"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("| slight_RotaryEncoder_Basic.ino"));
	pOut.println(F("|   Basic Example for slight_RotaryEncoder library"));
	pOut.println(F("|"));
	pOut.println(F("| dream on & have fun :-)"));
	pOut.println(F("|"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("|"));
	//pOut.println(F("| Version: Nov 11 2013  20:35:04"));
	pOut.print(F("| version: "));
	pOut.print(F(__DATE__));
	pOut.print(F("  "));
	pOut.print(F(__TIME__));
	pOut.println();
	pOut.println(F("|"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println();
	
	//pOut.println(__DATE__); Nov 11 2013
	//pOut.println(__TIME__); 20:35:04
}

/** Serial.print to Flash: Notepad++ Replace RegEx
	Find what:		Serial.print(.*)\("(.*)"\);
	Replace with:	Serial.print\1(F("\2"));
**/

/**************************************************************************************************/
/** definitions (gloabl)                                                                         **/
/**************************************************************************************************/

/**************************************************/
/**  slight Rotary Encoder                       **/
/**************************************************/

const byte bPin_Rotary_1_A = A0;	// PB4
const byte bPin_Rotary_1_B = A1;	// PD6

/* constructor
slight_RotaryEncoder(
	byte cbID_New,
	byte cbPin_A_New,
	byte cbPin_B_New,
	byte cbPulsPerStep_New,
	tcbfOnEvent cbfCallbackOnEvent_New
);
*/
slight_RotaryEncoder myEncoder1(
	1, // byte cbID_New,
	bPin_Rotary_1_A, // byte cbPin_A_New,
	bPin_Rotary_1_B, // byte cbPin_B_New,
	2, // byte cbPulsPerStep_New,
	myCallback_onEvent // tcbfOnEvent cbfCallbackOnEvent_New
);


word wCounterTest1 = 1000;


/**************************************************/
/**  DebugOut                                    **/
/**************************************************/

boolean bLEDState = 0;
const byte cbID_LED_Info = 9; //D9

unsigned long ulDebugOut_LiveSign_TimeStamp_LastAction	= 0;
const word cwDebugOut_LiveSign_UpdateInterval			= 1000; //ms

boolean bDebugOut_LiveSign_Serial_Enabled	= 0;
boolean bDebugOut_LiveSign_LED_Enabled		= 1;


/**************************************************/
/** other things...                              **/
/**************************************************/




/**************************************************************************************************/
/** functions                                                                                    **/
/**************************************************************************************************/

/************************************************/
/**  helper functions                          **/
/************************************************/

// setup Timer1 and interrupt on Compare A Match for 1ms
void setupTimer1() {
	
	/**
	  *
	  *
	  *
	  *  Table 14-5. Waveform Generation Mode Bit Description (Page 131 Atmel Atmega32U4 Datasheet)
	  *  (1)
	  *  Mode    WGMn3    WGMn2    WGMn1    WGMn0    Timer/Counter                      TOP        Update of    TOVn Flag
	  *                   CTCn     PWMn1    PWMn0    Mode of Operation                             OCRn x at    Set on
	  *   0        0        0        0        0      Normal                              0xFFFF    Immediate    MAX
	  *   1        0        0        0        1      PWM, Phase Correct,  8-bit          0x00FF    TOP          BOTTOM
	  *   2        0        0        1        0      PWM, Phase Correct,  9-bit          0x01FF    TOP          BOTTOM
	  *   3        0        0        1        1      PWM, Phase Correct, 10-bit          0x03FF    TOP          BOTTOM
	  *   4        0        1        0        0      CTC                                 OCRnA     Immediate    MAX
	  *   5        0        1        0        1      Fast PWM,  8-bit                    0x00FF    TOP          TOP
	  *   6        0        1        1        0      Fast PWM,  9-bit                    0x01FF    TOP          TOP
	  *   7        0        1        1        1      Fast PWM, 10-bit                    0x03FF    TOP          TOP
	  *   8        1        0        0        0      PWM, Phase and Frequency Correct    ICRn      BOTTOM       BOTTOM
	  *   9        1        0        0        1      PWM, Phase and Frequency Correct    OCRnA     BOTTOM       BOTTOM
	  *  10        1        0        1        0      PWM, Phase               Correct    ICRn      TOP          BOTTOM
	  *  11        1        0        1        1      PWM, Phase               Correct    OCRnA     TOP          BOTTOM
	  *  12        1        1        0        0      CTC                                 ICRn      Immediate    MAX
	  *
	  *
	  *
	  *
	  **/
	
	// clear Timer Counter Control Register A and B
	TCCR1A = 0;
	TCCR1B = 0;
	
	// set to Timer Mode 4 (CTC = Clear Timer on Compare Match); Prescaler  /64
	
	TCCR1A = (0 << WGM10)	// Waveform Generation Mode
		   | (0 << WGM11)	// Waveform Generation Mode
		   | (0 << COM1C0)	// Compare Output Mode channel C
		   | (0 << COM1C1)	// Compare Output Mode channel C
		   | (0 << COM1B0)	// Compare Output Mode channel B
		   | (0 << COM1B1)	// Compare Output Mode channel B
		   | (0 << COM1A0)	// Compare Output Mode channel A
		   | (0 << COM1A1);	// Compare Output Mode channel A
	
	TCCR1B = (0 << ICNC1)	// Input Capture Noise Canceler
		   | (0 << ICES1)	// Input Capture Edge Select
		   | (0 << 0    )	// Reserved.
		   | (1 << WGM12)	// Waveform Generation Mode
		   | (0 << WGM13)	// Waveform Generation Mode
		   | (0 <<  CS12)	// CSn2 0 0 0 0 1 1 1 1
		   | (1 <<  CS11)	// CSn1 0 0 1 1 0 0 1 1
		   | (1 <<  CS10);	// CSn0 0 1 0 1 0 1 0 1
	/**                             | | | | | | | | _2_1_0_
	  *                             | | | | | | | *- 1 1 1 External clock source on Tn pin. Clock on rising edge
	  *                             | | | | | | *--- 1 1 0 External clock source on Tn pin. Clock on falling edge
	  *                             | | | | | *----- 1 0 1 clkI/O/1024 (From prescaler)
	  *                             | | | | *------- 1 0 0 clkI/O/256 (From prescaler)
	  *                             | | | *--------- 0 1 1 clkI/O/64 (From prescaler)
	  *                             | | *----------- 0 1 0 clkI/O/8 (From prescaler)
	  *                             | *------------- 0 0 1 clkI/O/1 (No prescaling)
	  *                             *--------------- 0 0 0 No clock source. (Timer/Counter stopped)
	  * Table 14-6. Clock Select Bit Description
	  * 
	  **/
	
	OCR1A =  249;									// compare A register value (1000 * clock speed / 1024)
	TIMSK1 = _BV (OCIE1A);							// interrupt on Compare A Match
	
}


/************************************************/
/**  Debug things                              **/
/************************************************/

// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/************************************************/
/** rotary encoder                             **/
/************************************************/

void myCallback_onEvent(slight_RotaryEncoder *pInstance, byte bEvent) {
	// react on events:
	switch (bEvent) {
		// rotation
		case slight_RotaryEncoder::event_Rotated : {
			// get current data
			int iTemp_Steps = (*pInstance).getSteps();
			// clear data
			(*pInstance).resetData();
			
			Serial.print(F("Instance ID:"));
			Serial.println((*pInstance).getID());
			
			Serial.print(F("\t Event: "));
			(*pInstance).printEvent(Serial, bEvent);
			Serial.println();
			
			Serial.print(F("\t steps: "));
			Serial.println(iTemp_Steps);

		} break;
		// currently there are no other events fired.
	} //end switch
}




/**************************************************/
/**                                              **/
/**************************************************/






/****************************************************************************************************/
/** Setup                                                                                          **/
/****************************************************************************************************/
void setup() {
	
	/************************************************/
	/** Initialise PINs                            **/
	/************************************************/
		
		//LiveSign
		pinMode(cbID_LED_Info, OUTPUT);
		digitalWrite(cbID_LED_Info, HIGH);
		
		// as of arduino 1.0.1 you can use INPUT_PULLUP
		
		
	/************************************************/
	/** init serial                                **/
	/************************************************/
		
		// for ATmega32U4 devices:
		#if defined (__AVR_ATmega32U4__)
			//wait for arduino IDE to release all serial ports after upload.
			delay(2000);
		#endif
		
		Serial.begin(115200);
		
		// for ATmega32U4 devices:
		#if defined (__AVR_ATmega32U4__)
			// Wait for Serial Connection to be Opend from Host or 6second timeout
			unsigned long ulTimeStamp_Start = millis();
			while( (! Serial) && ( (millis() - ulTimeStamp_Start) < 6000 ) ) {
				1;
			}
		#endif
		
		Serial.println();
		
		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());
		
		
	/************************************************/
	/** Welcom                                     **/
	/************************************************/
		
		print_info(Serial);
		
	/************************************************/
	/** setup Timer1                               **/
	/************************************************/
		
		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());
		
		Serial.println(F("setup Timer1:"));{
			setupTimer1();
		}
		Serial.println(F("\t finished."));
		
	/************************************************/
	/** setup RotaryEncoders                       **/
	/************************************************/
		
		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());
		
		Serial.println(F("setup RotaryEncoders:"));{
			
			Serial.println(F("\t myEncoder1.begin()"));
			myEncoder1.begin();
			
		}
		Serial.println(F("\t finished."));
		
	/************************************************/
	/** GO                                         **/
	/************************************************/
		
		Serial.println(F("Loop:"));
		
} /** setup **/


/****************************************************************************************************/
/** Interrupt Service Routin                                                                       **/
/****************************************************************************************************/
ISR(TIMER1_COMPA_vect) {
	// read rotary
	myEncoder1.updateClassic();
	//myEncoder1.updateGray(); // experimental
	
}



/****************************************************************************************************/
/** Main Loop                                                                                      **/
/****************************************************************************************************/
void loop() {
	
	/************************************************/
	/** read rotary encoders                       **/
	/************************************************/
		
		// update event system
		myEncoder1.update();
		
	/**************************************************/
	/** Debug Out                                    **/
	/**************************************************/
		
		if ( (millis() - ulDebugOut_LiveSign_TimeStamp_LastAction) > cwDebugOut_LiveSign_UpdateInterval) {
			ulDebugOut_LiveSign_TimeStamp_LastAction = millis();
			
			if ( bDebugOut_LiveSign_Serial_Enabled ) {
				Serial.print(millis());
				Serial.print(F("ms;"));
				Serial.print(F("  free RAM = "));
				Serial.println(freeRam());
			}
			
			if ( bDebugOut_LiveSign_LED_Enabled ) {
				bLEDState = ! bLEDState;
				if (bLEDState) {
					//set LED to HIGH
					digitalWrite(cbID_LED_Info, HIGH);
				} else {
					//set LED to LOW
					digitalWrite(cbID_LED_Info, LOW);
				}
			}
			
		}
		
	/**************************************************/
	/**                                              **/
	/**************************************************/
		
} /** loop **/


/****************************************************************************************************/
/** THE END                                                                                        **/
/****************************************************************************************************/