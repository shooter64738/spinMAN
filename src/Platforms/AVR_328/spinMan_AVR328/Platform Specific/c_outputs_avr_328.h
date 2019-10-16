#ifndef __C_OUTPUTS_328_P_H__
#define __C_OUTPUTS_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../../../core/c_enumerations.h"

//#include "../../../../c_controller.h"

//#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define PWM_OUTPUT_PIN PB1 //(pin 9 )
#define PWM_DIR_PORT DDRB
#define PWM_DIR_PORT_ID DDB1
#define RPM_REFERNCE_PIN 0// PD6 //(pin 6 )

#define DIRECTION_PORT PORTB
#define DIRECTION_PORT_DIRECTION DDRB
#define DIRECTION_PORT_PIN_ADDRESS PINB
#define OUT_DIRECTION_PIN PB5			//Nano pin D13
#define BRAKE_PIN PB4			//Nano pin D12

namespace HardwareAbstractionLayer
{
	class Outputs
	{
		public:
		
		static void initialize();
		static void configure_pwm_output_timer();
		static void set_direction(Spin::Enums::e_directions direction);
		static void enable_output();
		static void update_output(uint16_t value);
		static void disable_output();
	};
};
#endif

