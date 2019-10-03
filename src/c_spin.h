/*
* c_spin.h
*
* Created: 9/30/2019 7:34:49 PM
* Author: Family
*/


#ifndef __C_SPIN_INPUT_CONTROLS_H__
#define __C_SPIN_INPUT_CONTROLS_H__

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

#define PWM_OUTPUT_PIN PD6 //(pin 6 )

#define encoder_ticks_per_rev 400.0
#define frq_gate_time_ms 500.0
#define rpm_gate_time_ms 125
#define rpm_buffer_size 8
#define milliseconds_per_second 1000
#define gate_factor milliseconds_per_second/frq_gate_time_ms

/*
Mode ALL:
Direction_Pin: High = CW, Low = CCW
Enable_Pin: High = Enable, Low = Disable

Mode 0:	Mode_Pin is HIGH
Step_Pin frequency indicates the RPM rate 1hz=1rpm, 10,000hz=10,000rpm
Mode 1:	Mode_Pin is LOW
Step_Pin: Each time pin changes from low to high the motor will move 1 encoder division
If motor shaft is moved from its rest position, it will be moved back
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"
#include <stdint.h>

namespace Spin
{
	namespace Input
	{
		class Controls
		{
			public:
			struct s_flag_ui8
			{
				uint8_t Value;
				uint8_t Dirty;
			};
			struct s_flag_ui32
			{
				uint32_t Value;
				uint8_t Dirty;
			};
			struct s_flag_ui16_arr
			{
				uint16_t Value_Buffer[rpm_buffer_size];
				uint16_t Value;
				uint8_t Dirty;
			};
			struct s_flags
			{
				s_flag_ui32 Step;
				s_flag_ui8 Direction;
				s_flag_ui8 Enable;
				s_flag_ui8 Mode;
				s_flag_ui32 Index;
				s_flag_ui16_arr Rpm;
				s_flag_ui32 Encoder;
			};
			
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
					
					return cv_control_variable;
				}
				
				
			};
			
			//variables
			public:
			static c_Serial host_serial;
			static s_flags Control;
			static s_pid_terms as_position;
			static s_pid_terms as_velocity;
			protected:
			private:

			//functions
			public:
			
			static void run();
			static float update_pid(uint32_t target, uint32_t current);
			static void update_vitals();
			static void initialize();
			static void set_pid_defaults();
			static void setup_pulse_inputs();
			static void timer_re_start();
			static void timer1_reset();
			static void timer2_reset();
			static void setup_pwm_out();
			static void setup_control_inputs();
			static void setup_encoder_capture();
			static void encoder_update();
			protected:
			private:

		}; //c_spin
	};
};
#endif //__C_SPIN_H__
