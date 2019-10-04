/*
* c_input.h
*
* Created: 10/3/2019 4:14:45 PM
* Author: jeff_d
*/


#ifndef __C_INPUT_H__
#define __C_INPUT_H__

#define CONTROl_PORT PORTB
#define CONTROl_PORT_DIRECTION DDRB
#define CONTROL_PORT_PIN_ADDRESS PINB
#define DIRECTION_PIN PINB0	//Nano pin D8
#define MODE_PIN_A PINB1			//Nano pin D9
#define MODE_PIN_B PINB2			//Nano pin D9
#define ENABLE_PIN PINB3		//Nano pin D11

#define STEP_PORT PORTD
#define STEP_PORT_DIRECTION DDRD
#define STEP_PORT_PIN_ADDRESS PIND
#define STEP_PIN PIND7			//Nano pin D7
#define STEP_PIN_ON_TIMER PIND5			//Nano pin D5
#define INDEX_PIN_ON_TIMER PIND4			//Nano pin D5

#define ENCODER_TICKS_PER_REV 400.0
#define FRQ_GATE_TIME_MS 500.0
#define RPM_GATE_TIME_MS 125.0




#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"
#include <stdint.h>
#include "c_controller.h"

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
			int32_t sensed_rpm;
			int32_t encoder_count;
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
		static void update_rpm();
		static void update_time_keeping();
		static void setup_pulse_inputs();
		static void timer_re_start();
		static void timer1_reset();
		static void timer2_reset();
		static void setup_control_inputs();
		static void setup_encoder_capture();
		static void encoder_update();
		protected:
		private:

	}; //c_spin
};

#endif
