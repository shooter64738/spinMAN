#ifndef __C_INPUTS_328_P_H__
#define __C_INPUTS_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

#define CONTROl_PORT PORTB
#define CONTROl_PORT_DIRECTION DDRB
#define CONTROL_PORT_PIN_ADDRESS PINB
#define DIRECTION_PIN PINB0	//Nano pin D8
#define MODE_PIN_A PINB1			//Nano pin D9
#define MODE_PIN_B PINB2			//Nano pin D10
#define ENABLE_PIN PINB3		//Nano pin D11

#define STEP_PORT PORTD
#define STEP_PORT_DIRECTION DDRD
#define STEP_PORT_PIN_ADDRESS PIND
#define STEP_PIN PIND7			//Nano pin D7
#define STEP_PIN_ON_TIMER PIND5			//Nano pin D5
#define INDEX_PIN_ON_TIMER PIND4			//Nano pin D4


namespace HardwareAbstractionLayer
{
	class Inputs
	{
		public:
		
		static void get_rpm();
		static void get_set_point();
		static void initialize();
		static void configure();
		static void configure_encoder_z_index();
		static void configure_encoder_simple_int_a();
		static void configure_encoder_simple_int_b();
		static void configure_encoder_quadrature();
		static void synch_hardware_inputs();
		static void synch_hardware_inputs(uint8_t current);
		static uint8_t get_intervals();
		static void check_intervals();
		static uint8_t get_encoder_active();
		static void update_encoder_for_quad();
	};
};
#endif

