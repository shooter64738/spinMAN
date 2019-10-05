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
#define INDEX_PIN_ON_TIMER PIND4			//Nano pin D5

namespace HardwareAbstractionLayer
{
	class Inputs
	{
		public:
		
		static void get_rpm();
		static void initialize();
		static void configure();
		static void synch_hardware_inputs();
		static void synch_hardware_inputs(uint8_t current);
		static void update_encoder();
	};
};
#endif
