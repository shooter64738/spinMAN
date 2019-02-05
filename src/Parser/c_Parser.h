/*
* c_Spindle.h
*
* Created: 3/6/2018 1:21:49 PM
* Author: jeff_d
*/


#ifndef __C_SPINDLE_H__
#define __C_SPINDLE_H__
#include "../Serial/c_Serial.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "../NGC_RS274/NGC_Codes.h"
#include "../NGC_RS274/NGC_Errors.h"
#include "../NGC_RS274/NGC_Groups.h"

#define CYCLE_LINE_LENGTH 128
#define MESSAGE_LENGTH 20
#define SPINDLE_SPEED_PIN PORTB1

#define PARSER_DIRECTION_CW 1
#define PARSER_DIRECTION_CCW -1
#define PARSER_DIRECTION_OFF 0

class c_Parser
{
	typedef struct
	{
		/*
		in g95 feed mode the target rpm is calculated by the feed rate.
		in all other feed modes, the spindle speed is simply dictated by the S word address value.
		*/
		float target_rpm;//<--rpm requested in g-code
		float feed_rate;//<--feed rate requested in gcode
		int8_t rotation_direction; //<--cw,ccw,off
		float roation_angle;
		uint8_t feed_mode;
	} s_spindle;
	
	//variables
	public:
	
	static s_spindle spindle_input;

	protected:
	private:

	//functions
	public:
	static void Initialize();
	static int16_t CheckInput();
	static char Message[MESSAGE_LENGTH];
	protected:
	
	private:
	static char Line[CYCLE_LINE_LENGTH];
	static int16_t _ParseValues();
	
	static float _Evaluate_Address(char* Data);
	static int16_t _groupWord(char Word, float Address);
	
}; //c_Spindle
//
//c_Spindle();
//c_Spindle(c_Serial);
//~c_Spindle();
#endif //__C_SPINDLE_H__
