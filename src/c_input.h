/*
* c_input.h
*
* Created: 10/3/2019 4:14:45 PM
* Author: jeff_d
*/


#ifndef __C_INPUT_H__
#define __C_INPUT_H__





#include "Serial\c_Serial.h"
#include <stdint.h>

#include "c_enumerations.h"


#define TIMER_PRESCALE_MASK (1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
static const float INV_ENCODER_TICKS_PER_REV = 1.0 / 400.0; //<--encoder ticks in a revolution
#define PRE_SCALER 128
static const float TIMER_FRQ_HZ = 1000;
#define TIMER_TICKS_PER_SECOND (F_CPU/PRE_SCALER)/TIMER_FRQ_HZ //125000; //<--frequency in hz of the timer with the selected prescaler
#define MILLISECONDS_PER_SECOND 1000

static const float PID_GATE_TIME_MS = TIMER_FRQ_HZ / 5; //fast
static const float RPM_GATE_TIME_MS = TIMER_FRQ_HZ / 10; //faster
static const float SET_GATE_TIME_MS = TIMER_FRQ_HZ; //slowest


#define PID_INTERVAL_BIT 0
#define RPM_INTERVAL_BIT 1
#define ONE_INTERVAL_BIT 2

namespace Spin
{
	class Input
	{
		public:
		
		struct s_flags
		{
			uint32_t step_counter;
			Spin::Enums::e_drive_modes in_mode;
			Spin::Enums::e_drive_states enable;
			Spin::Enums::e_directions direction;
			uint32_t index;
			int32_t sensed_rpm = 0;
			int32_t sensed_position = 0;
		};
		
		
		
		//variables
		public:
		static c_Serial host_serial;
		static s_flags Controls;

		//static uint32_t Flags;
		
		
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void check_input_states();
		protected:
		private:

	}; //c_spin
};

#endif
