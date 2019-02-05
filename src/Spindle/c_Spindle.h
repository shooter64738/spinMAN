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

#define DIRECTION_CW 1
#define DIRECTION_CCW -1
#define DIRECTION_OFF 0

typedef struct
	{
		/*
		in g95 feed mode the target rpm is calculated by the feed rate.
		in all other feed modes, the spindle speed is simply dictated by the s word address value.
		*/
		float target_rpm;//<--rpm requested in g-code
		float feed_rate;//<--feed rate requested in gcode
		int8_t rotation_direction; //<--cw,ccw,off
		float roation_angle;
	} s_spindle;

class c_Spindle
{
	
	//variables
	public:
	
	static s_spindle spindle_input;

	protected:
	private:

	//functions
	public:
	
	void ProcessLine();
	static char Message[MESSAGE_LENGTH];
	protected:
	
	private:
	static char Line[CYCLE_LINE_LENGTH];
	static int _ParseValues();
	static c_Serial host_serial;
	static float _Evaluate_Address(char* Data);
	static int _groupWord(char Word, float Address);
	static void Spindle_To_Speed(int Percent);
}; //c_Spindle
//
//c_Spindle();
//c_Spindle(c_Serial);
//~c_Spindle();
#endif //__C_SPINDLE_H__
