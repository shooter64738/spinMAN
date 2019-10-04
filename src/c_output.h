/*
* c_output.h
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#ifndef __C_OUTPUT_H__
#define __C_OUTPUT_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"
#include <stdint.h>

#define MAX_INT INT16_MAX
#define MAX_LONG INT32_MAX
#define MAX_I_TERM (MAX_LONG / 2)

#define SCALING_FACTOR 8
#define PWM_OUTPUT_PIN PD6 //(pin 6 )

namespace Spin
{
	class Output
	{
		public:
		struct s_pid_terms
		{
			int32_t kP;
			int32_t kI;
			int32_t kD;
			float proportional;
			float integral;
			float derivative;
			float preError;
			int16_t max;
			int16_t min;
			int32_t output;
			void reset()
			{
				
			}
			
			int32_t get_pid(int32_t setPoint,int32_t processValue)
			{
				float err = setPoint-processValue;
				float fkp = kP*.01;
				float fki = kI*.01;
				float fkd = kD*.01;
				
				proportional = fkp * err;
				// track error over time, scaled to the timer interval
				integral += (err * 0.125);
				if (integral>INT16_MAX) integral = INT16_MAX;
				else if (integral<INT16_MIN) integral = INT16_MIN;
				// determine the amount of change from the last time checked
				derivative = (err-preError);
				
				if (derivative>INT16_MAX) derivative = INT16_MAX;
				else if (derivative<INT16_MIN) derivative = INT16_MIN;
				// calculate how much to drive the output in order to get to the
				// desired setpoint.

				output = proportional + (fki*integral) + (fkp*derivative);
				
				//if (output<max) output = max;
				//else if (output<min) output = min;
				// remember the error for the next time around.
				preError = err;
				
				if (preError>INT16_MAX) preError = INT16_MAX;
				else if (preError<INT16_MIN) preError = INT16_MIN;
				
				return max-output;
			}
		};
		static s_pid_terms as_position;
		static s_pid_terms as_velocity;
		static s_pid_terms * active_pid_mode;
		protected:
		private:

		public:
		static void initialize();
		static float update_pid(uint32_t target, uint32_t current);
		static void set_pid_defaults();
		static void set_drive_state(uint8_t state);
		static void setup_pwm_timer();
		static void set_mode(uint8_t mode);
	};
};
#endif //__C_OUTPUT_H__
