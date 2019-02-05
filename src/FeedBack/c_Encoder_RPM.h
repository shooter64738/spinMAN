/*
* grblRTC.c
*
* Created: 1/1/2018 12:03:25 PM
* Author : jeff_d
*/
#pragma once
#ifndef c_FeedBack_h
#define c_FeedBack_h
#include <avr/io.h>
#define F_CPU 16000000UL
#include "util/delay.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>    // Needed to use interrupts
//#include "../Machine/c_Axis.h"
//#include "../Machine/c_Machine.h"
//#include "../Configuration/Config.h"

class c_Encoder_RPM
{
	public:
	
		
	static void Encoder_Trigger();
	static float CurrentAngle_DEC();
	static float CurrentAngle_RAD();
	static float CurrentAngle_DEG();
	static float CurrentRPM();
	static uint32_t AverageRPM();
	static void SetAverageRPM();
	static float CurrentRotation();
	static void Initialize(uint16_t encoder_ticks_per_rev);
	static void Encoder_Update();
	static int32_t Encoder_Position();
	static uint16_t encoder_ticks_per_rev;
		

	//private:
	static float encoder_rpm_multiplier;
	static float encoder_angle_multiplier;
};
#endif
