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


#define PWM_OUTPUT_PIN PD6 //(pin 6 )

namespace Spin
{
	namespace Control
	{
		class Output
		{
			public:
			struct s_pid_terms
			{
				float Kp;
				float Ki;
				float Kd;
				float set_last;
				float actual_last;
				float cv_control_variable;
				float pv_process_variable;
				float p_proportional_variable = 0;
				float i_integral_variable;
				float d_derivative_variable;
				float e_error_last;
				float e_error;
				float t_time;
				int8_t drive_direction;
				int16_t Max_Val;
				int16_t Min_Val;
				uint8_t invert_output;
				
				void reset()
				{
					Kp = 0;
					Ki = 0;
					Kd = 0;
					cv_control_variable = 0;
					pv_process_variable = 0;
					p_proportional_variable = 0;
					i_integral_variable = 0;
					d_derivative_variable = 0;
					e_error_last = 0;
					e_error = 0;
					Max_Val = 0;
					Min_Val = 0;
					set_last = 0;
					actual_last = 0;
					t_time = 0;
				}
				
				int32_t get_pid(int32_t set_point,int32_t actual)
				{
					set_last = set_point;
					actual_last = actual;
					
					e_error_last = e_error;
					
					e_error = set_point - actual;
					p_proportional_variable = Kp * e_error;
					i_integral_variable += (e_error * (t_time!=0?t_time:1));
					
					if(i_integral_variable > Max_Val) i_integral_variable= Max_Val;
					else if(i_integral_variable < Min_Val) i_integral_variable= Min_Val;
					
					d_derivative_variable = (e_error-e_error_last)/(t_time!=0?t_time:1);
					cv_control_variable = (Kp*e_error) + (Ki*i_integral_variable) + (Kd * d_derivative_variable);
					
					
					//return cv_control_variable;
					
					if (cv_control_variable > Max_Val) cv_control_variable = Max_Val;
					else if (cv_control_variable< Min_Val) cv_control_variable = Min_Val;
					
					drive_direction = (cv_control_variable<1)?-1:1;

					return cv_control_variable;
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
};
#endif //__C_OUTPUT_H__
