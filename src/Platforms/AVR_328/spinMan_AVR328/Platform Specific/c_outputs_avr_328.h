#ifndef __C_OUTPUTS_328_P_H__
#define __C_OUTPUTS_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define RPM_REFERNCE_PIN 0// PD6 //(pin 6 )

namespace HardwareAbstractionLayer
{
	class Outputs
	{
		public:
		
		static void initialize();
		static void configure();
		static void enable_output();
		static void update_output(uint16_t value);
		static void disable_output();
		static void disable_output(uint16_t off_value);
	};
};
#endif

