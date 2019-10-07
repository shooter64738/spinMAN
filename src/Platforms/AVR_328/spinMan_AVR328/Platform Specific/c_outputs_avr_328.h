#ifndef __C_OUTPUTS_328_P_H__
#define __C_OUTPUTS_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../../../c_controller.h"

#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define RPM_REFERNCE_PIN 0// PD6 //(pin 6 )

#define DIRECTION_PORT PORTB
#define DIRECTION_PORT_DIRECTION DDRB
#define DIRECTION_PORT_PIN_ADDRESS PINB
#define FORWARD_PIN PINB5			//Nano pin D13
#define REVERSE_PIN PINB4			//Nano pin D12

namespace HardwareAbstractionLayer
{
	class Outputs
	{
		public:
		
		static void initialize();
		static void set_direction(uint8_t direction);
		static void enable_output();
		static void update_output(uint16_t value);
		static void disable_output();
		static void disable_output(uint16_t off_value);
	};
};
#endif

