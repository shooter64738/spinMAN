/*
* c_input.h
*
* Created: 10/3/2019 4:14:45 PM
* Author: jeff_d
*/


#ifndef __C_INPUT_H__
#define __C_INPUT_H__





#include "../Serial/c_Serial.h"
#include <stdint.h>
#include "../hardware_def.h"
#include "../core/c_enumerations.h"


#define TIMER_PRESCALE_MASK (1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
//static const float INV_ENCODER_TICKS_PER_REV = 1.0 / 400.0; //<--encoder ticks in a revolution
#define PRE_SCALER 128
#define TIMER_FRQ_HZ 1000
#define TIMER_TICKS_PER_SECOND (F_CPU/PRE_SCALER)/TIMER_FRQ_HZ //125000; //<--frequency in hz of the timer with the selected prescaler
#define MILLISECONDS_PER_SECOND 1000
#define RPM_PERIODS_IN_INTERVAL 10.0
#define PID_PERIODS_IN_INTERVAL 5.0
#define PID_GATE_TIME_MS TIMER_FRQ_HZ / PID_PERIODS_IN_INTERVAL //fast
#define RPM_GATE_TIME_MS TIMER_FRQ_HZ / RPM_PERIODS_IN_INTERVAL //faster
#define SET_GATE_TIME_MS TIMER_FRQ_HZ //slowest
#define RPT_GATE_TIME_MS TIMER_FRQ_HZ /2


#define PID_INTERVAL_BIT 0
#define RPM_INTERVAL_BIT 1
#define ONE_INTERVAL_BIT 2
#define RPT_INTERVAL_BIT 3



namespace Spin
{
	class Input
	{
		public:
		
		struct s_flags
		{
			uint32_t target;
			Spin::Enums::e_drive_modes in_mode;
			Spin::Enums::e_drive_states enable;
			Spin::Enums::e_directions direction;
			//uint32_t index;
			//int32_t sensed_position; //<--Removed. Use extern_encoder__count instead
			
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
