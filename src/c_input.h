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
#define Direction_Pin PINB0	//Nano pin D8
#define Mode_Pin PINB1			//Nano pin D9
#define Enable_Pin PINB2		//Nano pin D10

#define STEP_PORT PORTD
#define STEP_PORT_DIRECTION DDRD
#define STEP_PORT_PIN_ADDRESS PIND
#define Step_Pin PIND7			//Nano pin D7
#define Step_Pin_on_Timer PIND5			//Nano pin D5
#define Index_Pin_on_Timer PIND4			//Nano pin D5

#define encoder_ticks_per_rev 400.0
#define frq_gate_time_ms 500.0
#define rpm_gate_time_ms 125.0
#define milliseconds_per_second 1000



#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"
#include <stdint.h>

namespace Spin
{
	namespace Control
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
				s_flag_ui32 Step;
				s_flag_ui8 Direction;
				s_flag_ui8 Enable;
				s_flag_ui8 Mode;
				s_flag_ui32 Index;
				s_flag_ui16 Rpm;
				s_flag_ui32 Encoder;
			};
			
			
			
			//variables
			public:
			static c_Serial host_serial;
			static s_flags Actions;
			
			protected:
			private:

			//functions
			public:
			
			static void run();
			
			static void update_rpm();
			static void initialize();
			
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
};

#endif
