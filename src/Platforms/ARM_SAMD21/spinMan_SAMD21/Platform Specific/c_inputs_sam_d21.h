#ifndef __C_INPUTS_SAM_D21_H__
#define __C_INPUTS_SAM_D21_H__

#include <stdint.h>

#define IN_TCCRA TCCR0A
#define IN_TCCRB TCCR0B
#define IN_TCNT TCNT0
#define IN_TIFR TIFR0
#define IN_TOV TOV0


#define CONTROl_PORT PORTC
#define CONTROl_PORT_DIRECTION DDRC
#define CONTROL_PORT_PIN_ADDRESS PINC
#define DIRECTION_PIN PINC0	//Nano pin A0
#define MODE_PIN_A PINC1    //Nano pin A1
#define MODE_PIN_B PINC2    //Nano pin A2
#define ENABLE_PIN PINC3    //Nano pin A3
#define ARM_PIN PINC4       //Nano pin A4

namespace HardwareAbstractionLayer
{
	class Inputs
	{
		public:
		static void get_set_point();
		static void initialize();
		static void configure_signal_input_timer();
		static void configure_interval_timer();
		static void configure();		
		static void synch_hardware_inputs();
		static void synch_hardware_inputs(uint8_t current);
		static uint8_t get_encoder_active();
		static void update_encoder_for_quad();
	};
};
#endif

