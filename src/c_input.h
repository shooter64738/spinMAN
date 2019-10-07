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
#include "c_controller.h"

#define PID_INTERVAL_BIT 0
#define RPM_INTERVAL_BIT 1
#define ONE_INTERVAL_BIT 2

namespace Spin
{
	class Input
	{
		public:
		struct s_flag_ui8
		{
			uint8_t Value;
			uint8_t Dirty;
		};
		struct s_flag_ui16
		{
			uint32_t Value;
			uint8_t Dirty;
		};
		struct s_flag_ui32
		{
			uint32_t Value;
			uint8_t Dirty;
		};
		
		struct s_flags
		{
			uint32_t step_counter;
			Spin::Controller::e_drive_modes in_mode;
			Spin::Controller::e_drive_states enable;
			Spin::Controller::e_directions direction;
			s_flag_ui32 Index;
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
		static void setup_pulse_inputs();
		static void timer_re_start();
		static void timer1_reset();
		static void timer2_reset();
		static void setup_control_inputs();
		static void setup_encoder_capture();		
		protected:
		private:

	}; //c_spin
};

#endif
