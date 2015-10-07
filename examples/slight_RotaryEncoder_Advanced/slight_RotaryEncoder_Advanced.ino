/**************************************************************************************************

	slight_RotaryEncoder_Advanced
		Test sketch for slight_RotaryEncoder library
		debugport: serial interface 115200baud

	hardware:
		Board:
		Arduino compatible board (tested with Leonardo compatible clone)

	libraries used:
		~ slight_RotaryEncoder


	written by stefan krueger (s-light),
		github@s-light.eu, http://s-light.eu, https://github.com/s-light/


	changelog / history
		09.01.2014 08:05 based on IOController_RotaryEncoder_HWTest_2.ino
		08.01.2014 08:40 cleaned up for example.
		16.03.2014 11:42 changed to use new onEvent system
		16.03.2014 15:11 moved basic acceleration system to library.
		16.03.2014 15:20 added testcase for testing update delay (menu option)


	TO DO:
		~ test



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
	pOut.println(F("| slight_RotaryEncoder_Advanced.ino"));
	pOut.println(F("|   Test sketch for slight_RotaryEncoder library"));
	pOut.println(F("|"));
	pOut.println(F("| This Sketch has a debug-menu:"));
	pOut.println(F("| send '?'+Return for help"));
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

const byte bPin_Rotary_1_A =  8;	// PB4
const byte bPin_Rotary_1_B = 12;	// PD6
const byte bPin_Rotary_2_A =  6;	// PD7
const byte bPin_Rotary_2_B =  4;	// PD4

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
	1, // byte cbPulsPerStep_New,
	myCallback_onEvent // tcbfOnEvent cbfCallbackOnEvent_New
);
slight_RotaryEncoder myEncoder2(
	2, // byte cbID_New,
	bPin_Rotary_2_A, // byte cbPin_A_New,
	bPin_Rotary_2_B, // byte cbPin_B_New,
	1, // byte cbPulsPerStep_New,
	myCallback_onEvent // tcbfOnEvent cbfCallbackOnEvent_New
);

word wCounterTest1 = 1000;
word wCounterTest2 = 1000;

boolean bDebugOption_SimulateHeavyMainLoop = false;

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
/** Menu Input                                   **/
/**************************************************/

// a string to hold new data
char  sMenu_Input_New[]				= "a:555:222";
// flag if string is complete
boolean bMenu_Input_New_FlagComplete	= false;

// string for Currently to process Command
char  sMenu_Command_Current[]		= "a:555:222";


/************************************************/
/**  Output system                             **/
/************************************************/
// DualWrite from pYro_65 read more at: http://forum.arduino.cc/index.php?topic=200975.0
class DualWriter : public Print{
	public:
		DualWriter( Print &p_Out1, Print &p_Out2 ) : OutputA( p_Out1 ), OutputB( p_Out2 ){}

		size_t write( uint8_t u_Data ) {
			OutputA.write( u_Data );
			OutputB.write( u_Data );
			return 0x01;
		}

	protected:
		Print &OutputA;
		Print &OutputB;
};

//DualWriter dwOUT( Serial, Serial1);



/**************************************************/
/** other things...                              **/
/**************************************************/




/**************************************************************************************************/
/** functions                                                                                    **/
/**************************************************************************************************/

/************************************************/
/**  helper functions                          **/
/************************************************/

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

	/* 16.9.2 Clear Timer on Compare Match (CTC) Mode
		( = Mode4; infos based on Datasheet ATmega328 Paragraph 16.9.2  Page123f )
		Interrupt on Compare A Match - time calculation:
		fOCnA = 'fclk_I/O' / (2*N*(1+OCRnA))

			fOCnA * (2*N*(1+OCRnA)) = 'fclk_I/O'		| * (2*N*(1+OCRnA))
			(2*N*(1+OCRnA)) = 'fclk_I/O' / fOCnA		| / fOCnA
			2*N*(1+OCRnA)  = ('fclk_I/O' / fOCnA)		| / (2*N)
			1+OCRnA  = ('fclk_I/O' / fOCnA) / (2*N)	| -1
			OCRnA  = (('fclk_I/O' / fOCnA) / (2*N) ) -1

		'fclk_I/O' = 16MHz
		N = 64
		OCR1A = 249

		fOCnA = 16MHz / (2*64*(1+249)) = 500Hz
		--> 1000ms / 500 = 2ms


		OCRnA = ((16MHz/500Hz) / (2*64)) -1
		OCRnA = 249

		OCRnA = ((16MHz/1000Hz) / (2*64)) -1
		OCRnA = 124

	*/

	// Set TOP
	OCR1A =  249;
	// interrupt on Compare A Match
	TIMSK1 = _BV (OCIE1A);

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


void printBinary8(byte bIn)  {

	for (unsigned int mask = 0b10000000; mask; mask >>= 1) {
		if (mask & bIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}

void printBinary12(word bIn)  {
	//                       B12345678   B12345678
	//for (unsigned int mask = 0x8000; mask; mask >>= 1) {
	for (unsigned int mask = 0b100000000000; mask; mask >>= 1) {
		if (mask & bIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}

void printBinary16(word wIn)  {

	for (unsigned int mask = 0b1000000000000000; mask; mask >>= 1) {
		if (mask & wIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}



/************************************************/
/**  Menu System                               **/
/************************************************/

// Modes for Menu Switcher
const uint8_t cbMenuMode_MainMenu	= 1;
const uint8_t cbMenuMode_SubMenu1	= 2;
uint8_t bMenuMode = cbMenuMode_MainMenu;


// SubMenu SetValues
void handle_SubMenu1(Print &pOut, char *caCommand) {
	pOut.println(F("SubMenu1:"));
	pOut.println(F("\t nothing here."));
	pOut.println(F("\t finished."));
	// exit submenu
	// reset state manschine of submenu
	// jump to main
	bMenuMode = cbMenuMode_MainMenu;
	bMenu_Input_New_FlagComplete = true;
}


// Main Menu
void handle_MainMenu(Print &pOut, char *caCommand) {
	/* pOut.print("sCommand: '");
	pOut.print(sCommand);
	pOut.println("'"); */
	switch (caCommand[0]) {
		case 'h':
		case 'H':
		case '?': {
			// help
			pOut.println(F("____________________________________________________________"));
			pOut.println();
			pOut.println(F("Help for Commands:"));
			pOut.println();
			pOut.println(F("\t '?': this help"));
			pOut.println(F("\t 'y': toggle DebugOut livesign print"));
			pOut.println(F("\t 'Y': toggle DebugOut livesign LED"));
			pOut.println(F("\t 'x': tests"));
			pOut.println();
			pOut.println(F("\t 'l': toggle Simulation of heavy main loop "));
			//pOut.println(F("\t 'f': DemoFadeTo(ID, value) 'f1:65535'"));
			pOut.println();
			pOut.println(F("\t 'set:' enter SubMenu1"));
			pOut.println();
			pOut.println(F("____________________________________________________________"));
		} break;
		case 'y': {
			pOut.println(F("\t toggle DebugOut livesign Serial:"));
			bDebugOut_LiveSign_Serial_Enabled = !bDebugOut_LiveSign_Serial_Enabled;
			pOut.print(F("\t bDebugOut_LiveSign_Serial_Enabled:"));
			pOut.println(bDebugOut_LiveSign_Serial_Enabled);
		} break;
		case 'Y': {
			pOut.println(F("\t toggle DebugOut livesign LED:"));
			bDebugOut_LiveSign_LED_Enabled = !bDebugOut_LiveSign_LED_Enabled;
			pOut.print(F("\t bDebugOut_LiveSign_LED_Enabled:"));
			pOut.println(bDebugOut_LiveSign_LED_Enabled);
		} break;
		case 'x': {
			// get state
			pOut.println(F("__________"));
			pOut.println(F("Tests:"));

			debug_time_measurement(pOut);

			pOut.println();

			pOut.println(F("__________"));
		} break;
		//--------------------------------------------------------------------------------
		case 'l': {
			pOut.println(F("\t toggle DebugOut livesign LED:"));
			bDebugOption_SimulateHeavyMainLoop = !bDebugOption_SimulateHeavyMainLoop;
			pOut.print(F("\t bDebugOption_SimulateHeavyMainLoop:"));
			pOut.println(bDebugOption_SimulateHeavyMainLoop);
		} break;
		/*case 'f': {
			pOut.print(F("\t DemoFadeTo "));
			byte bID = atoi(&caCommand[1]);
			pOut.print(bID);
			pOut.print(F(" : "));
			word wValue = atoi(&caCommand[3]);
			pOut.print(wValue);
			pOut.println();
			//demo_fadeTo(bID, wValue);
			pOut.print(F("\t demo for parsing values --> finished."));
		} break;*/
		//--------------------------------------------------------------------------------
		case 's': {
			// SubMenu1
			if ( (caCommand[1] == 'e') && (caCommand[2] == 't') && (caCommand[3] == ':') ) {
				//if full command is 'set:' enter submenu
				bMenuMode = cbMenuMode_SubMenu1;
				if(1){	//if ( caCommand[4] != '\0' ) {
					//full length command
					//handle_SetValues(pOut, &caCommand[4]);
				} else {
					bMenu_Input_New_FlagComplete = true;
				}
			}
		} break;
		//--------------------------------------------------------------------------------
		default: {
			pOut.print(F("command '"));
			pOut.print(caCommand);
			pOut.println(F("' not recognized. try again."));
			sMenu_Input_New[0] = '?';
			bMenu_Input_New_FlagComplete = true;
		}
	} //end switch

	//end Command Parser
}


// Menu Switcher
void menuSwitcher(Print &pOut, char *caCommand) {
	switch (bMenuMode) {
			case cbMenuMode_MainMenu: {
				handle_MainMenu(pOut, caCommand);
			} break;
			case cbMenuMode_SubMenu1: {
				handle_SubMenu1(pOut, caCommand);
			} break;
			default: {
				// something went wronge - so reset and show MainMenu
				bMenuMode = cbMenuMode_MainMenu;
			}
		} // end switch bMenuMode
}

// Check for NewLineComplete and enter menuSwitcher
// sets Menu Output channel (pOut)
void check_NewLineComplete() {
	// if SMenuCurrent is a full Line (terminated with \n) than parse things
	if (bMenu_Input_New_FlagComplete) {
		/*
		Serial.print(F("bMenu_Input_New_FlagComplete: sMenu_Input_New: '"));
		Serial.print(sMenu_Input_New);
		Serial.println(F("'"));
		Serial.print(F("   bState_UI: '"));
		Serial.print(bState_UI);
		Serial.println(F("'"));/**/

		// coppy to current buffer
		strcpy (sMenu_Command_Current, sMenu_Input_New);

		// reset input
		memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New)-1);
		bMenu_Input_New_FlagComplete = false;

		// run command
		menuSwitcher(Serial, sMenu_Command_Current);

	} // if bMenu_Input_New_FlagComplete
}

/************************************************/
/**  Serial Receive Handling                   **/
/************************************************/

void handle_SerialReceive() {
	// collect next input text
	while (Serial.available()) {
		// get the new byte:
		char charNew = (char)Serial.read();
		/*Serial.print(F("charNew '"));
		Serial.print(charNew);
		Serial.print(F("' : "));
		Serial.println(charNew, DEC);
		Serial.print(F("'\\n' : "));
		Serial.println('\n', DEC);
		Serial.print(F("'\\r' : '"));
		Serial.println('\r', DEC);*/

		/* http://forums.codeguru.com/showthread.php?253826-C-String-What-is-the-difference-between-n-and-r-n
			'\n' == 10 == LineFeed == LF
			'\r' == 13 == Carriag Return == CR
			Windows: '\r\n'
			Linux: '\n'
			Apple: '\r'
		*/
		// if the incoming character is a linefeed '\r' or newline '\n'
		//if ((charNew == '\r') || (charNew == '\n')) {
		if (charNew == '\n') {
			//clean up last received char if it is a '\r'
			if (sMenu_Input_New[strlen(sMenu_Input_New) - 1] == '\r') {
				sMenu_Input_New[strlen(sMenu_Input_New) - 1] = '\0';
			}
			//set complete flag
			bMenu_Input_New_FlagComplete = true;
		} else {
			// add it to the sMenu_Input_New:
			//sMenu_Input_New = sMenu_Input_New + charNew;
			// char array version
			//check for length..
			if (strlen(sMenu_Input_New) < sizeof(sMenu_Input_New) ) {
				sMenu_Input_New[strlen(sMenu_Input_New)] = charNew;
			} else {
				Serial.println(F("inputstring to long!"));
				// reset
				memset(sMenu_Input_New,'\0',sizeof(sMenu_Input_New)-1);
				bMenu_Input_New_FlagComplete = false;
			}
		}
	}
}


/************************************************/
/** rotary encoder                             **/
/************************************************/
/*
void myRotaryEncoder_callback_ValueChanged(byte bID, int iValue, byte bAccelerationDuration) {

	Serial.print(F("myEncoder"));
	Serial.print(bID);
	Serial.println(F(": "));
	Serial.print(F("\t steps: "));
	Serial.println(iValue);
	Serial.print(F("\t AccelDur: "));
	Serial.println(bAccelerationDuration);


	byte wAccelerationMap_in []		= {   0,  10,  11,  25,  26, 255};
	byte wAccelerationMap_out[]		= {  10,  10,   5,   5,   1,   1};
	byte wAccelerationMap_size		= 5;

	byte bAccelerationFactor =  multiMap(bAccelerationDuration, wAccelerationMap_in, wAccelerationMap_out, wAccelerationMap_size);

	Serial.print(F("\t bAccelerationFactor: "));
	Serial.println(bAccelerationFactor);

	int iStepsAccelerated =(iValue * bAccelerationFactor);

	Serial.print(F("\t iStepsAccelerated: "));
	Serial.println(iStepsAccelerated);

	Serial.print(F("\t counter: "));

	switch (bID) {
		case 1: {
			wCounterTest1 = wCounterTest1 + iStepsAccelerated;
			Serial.println(wCounterTest1);
		} break;
		case 2: {
			wCounterTest2 = wCounterTest2 + iStepsAccelerated;
			Serial.println(wCounterTest2);
		} break;
		default: {
			//
		}
	} // end switch ID

}
*/

void myCallback_onEvent(slight_RotaryEncoder *pInstance, byte bEvent) {
	// react on events:
	switch (bEvent) {
		// rotation
		case slight_RotaryEncoder::event_Rotated : {
			// get current data
			int iTemp_Steps = (*pInstance).getSteps();
			int iTemp_StepsAccelerated = (*pInstance).getStepsAccelerated();
			byte bTemp_AccelerationFactor = (*pInstance).getAccelerationFactor();
			// clear data
			(*pInstance).resetData();

			// do something with the data:
			Serial.print(F("Instance ID:"));
			Serial.println((*pInstance).getID());

			Serial.print(F("\t event: "));
			(*pInstance).printEvent(Serial, bEvent);
			Serial.println();

			Serial.print(F("\t steps: "));
			Serial.println(iTemp_Steps);

			Serial.print(F("\t steps accelerated: "));
			Serial.println(iTemp_StepsAccelerated);

			Serial.print(F("\t acceleration factor: "));
			Serial.println(bTemp_AccelerationFactor);

			Serial.print(F("\t counter: "));

			switch ((*pInstance).getID()) {
				case 1: {
					wCounterTest1 = wCounterTest1 + iTemp_StepsAccelerated;
					Serial.println(wCounterTest1);
				} break;
				case 2: {
					wCounterTest2 = wCounterTest2 + iTemp_StepsAccelerated;
					Serial.println(wCounterTest2);
				} break;
			} // end switch ID

		} break;
		// currently there are no other events fired.
	} //end switch event
}


/************************************************/
/** debug things                               **/
/************************************************/

void debug_time_measurement(Print &pOut) {

	pOut.println(F("~~~~~~~~~~~~~~~~~~~~"));

	pOut.println(F("time measurements:"));


	unsigned long ulIndex = 0;
	unsigned long ulLoopCount = 1000000; // max 4,294,967,295

	unsigned long ulTimeStamp_Start = millis();
	unsigned long ulTimeStamp_Stop = millis();
	unsigned long ulDuration = 0;

	pOut.print(F("\t ulLoopCount: "));
	pOut.println(ulLoopCount);

	/**/ {
		pOut.println(F("    myEncoder1.updateClassic();"));
		pOut.println(F("\t started..."));

		ulTimeStamp_Start = millis();
		for (ulIndex = 0; ulIndex <= ulLoopCount; ulIndex++) {
			myEncoder1.updateClassic();
		}
		ulTimeStamp_Stop = millis();

		ulDuration = ulTimeStamp_Stop - ulTimeStamp_Start ;

		pOut.println(F("\t end."));
		pOut.print(F("\t ulDuration: "));
		pOut.println(ulDuration);
	} /**/

	/**/ {
		pOut.println(F("    myEncoder1.updateGray();"));

		pOut.println(F("\t started..."));

		ulTimeStamp_Start = millis();
		for (ulIndex = 0; ulIndex <= ulLoopCount; ulIndex++) {
			myEncoder1.updateGray();
		}
		ulTimeStamp_Stop = millis();

		ulDuration = ulTimeStamp_Stop - ulTimeStamp_Start ;

		pOut.println(F("\t end."));
		pOut.print(F("\t ulDuration: "));
		pOut.println(ulDuration);
	} /**/

	pOut.println(F("~~~~~~~~~~~~~~~~~~~~"));
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

			//Serial.println(F("\t sub action"));
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

			Serial.println(F("\t myEncoder2.begin()"));
			myEncoder2.begin();
		}
		Serial.println(F("\t finished."));


	/************************************************/
	/** show Serial Commands                       **/
	/************************************************/

		// reset Serial Debug Input
		memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New)-1);
		//print Serial Options
		sMenu_Input_New[0] = '?';
		bMenu_Input_New_FlagComplete = true;


	/************************************************/
	/** GO                                         **/
	/************************************************/

		Serial.println(F("Loop:"));



} /** setup **/


/****************************************************************************************************/
/** Interrupt Service Routin                                                                       **/
/****************************************************************************************************/
ISR(TIMER1_COMPA_vect) {

	/** classic if / else logic **/
	myEncoder1.updateClassic();
	myEncoder2.updateClassic();
	/** **/

	/** GrayCode style **
	myEncoder1.updateGrayCode();
	myEncoder2.updateGrayCode();
	/** **/

}



/****************************************************************************************************/
/** Main Loop                                                                                      **/
/****************************************************************************************************/
void loop() {

	/**************************************************/
	/** Menu Input                                   **/
	/**************************************************/
		// Serial
		handle_SerialReceive();
		check_NewLineComplete();

		// Ohter Input Things:
		//handle_EthTelnet_Server();
		//check_NewLineComplete();


	/**************************************************/
	/** Timed things                                 **/
	/**************************************************/

		/*
		// every Nms
		if ( ( millis() - ulTimeStamp_LastAction ) > cwUpdateInterval) {
			ulTimeStamp_LastAction =  millis();

		}
		*/

	/************************************************/
	/** read rotary encoders                       **/
	/************************************************/
		// update event system
		myEncoder1.update();
		myEncoder2.update();

	/**************************************************/
	/** Debug Simulation                             **/
	/**************************************************/

		if (bDebugOption_SimulateHeavyMainLoop) {
			delay(100);
		}

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
