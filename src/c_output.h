/*
* c_output.h
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#ifndef __C_OUTPUT_H__
#define __C_OUTPUT_H__

#include "hardware_def.h"
#include "Serial\c_Serial.h"
#include <stdint.h>
#include "c_controller.h"



#define OUTPUT_OFF 255

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
			int32_t proportional;
			int32_t integral;
			int32_t derivative;
			int32_t preError;
			int16_t max;
			int16_t min;
			int32_t output;
			uint8_t invert_output;
			void reset()
			{
				kP = 0;
				kI = 0;
				kD = 0;
				proportional = 0;
				integral = 0;
				derivative = 0;
				preError = 0;
				max = 0;
				min = 0;
				output = OUTPUT_OFF;
				invert_output = 0;
			}

			int32_t get_pid(int32_t setPoint, int32_t processValue)
			{
				float err = setPoint - processValue;
				float fkp = kP*.001;
				float fki = kI*.001;
				float fkd = kD*.001;

				proportional = fkp * err;
				// track error over time, scaled to the timer interval
				integral += (err * 0.125);
				if (integral > INT16_MAX) integral = INT16_MAX;
				else if (integral<INT16_MIN) integral = INT16_MIN;
				// determine the amount of change from the last time checked
				derivative = (err - preError);

				if (derivative>INT16_MAX) derivative = INT16_MAX;
				else if (derivative<INT16_MIN) derivative = INT16_MIN;
				// calculate how much to drive the output in order to get to the
				// desired setpoint.

				output = proportional + (fki*integral) + (fkp*derivative);

				/*output = (invert_output ? max - output : output);

				if (output>max)
				{
					output = max;
				}
				else if (output<min)
				{
					output = min;
				}*/

				// remember the error for the next time around.
				preError = err;

				if (preError>INT16_MAX) preError = INT16_MAX;
				else if (preError < INT16_MIN) preError = INT16_MIN;

				return output;
			}
		};

		struct s_flags
		{
			uint32_t step_counter;
			Spin::Controller::e_drive_modes out_mode;
			Spin::Controller::e_drive_states enable;
			Spin::Controller::e_directions direction;
		};



		static s_flags Controls;

		static s_pid_terms as_position;
		static s_pid_terms as_velocity;
		static s_pid_terms as_torque;
		static s_pid_terms * active_pid_mode;

	protected:
	private:

	public:
		static void initialize();
		static void set_output();
		static void set_pid_defaults();
		static void set_drive_state(Spin::Controller::e_drive_states state);
		static void set_mode(Spin::Controller::e_drive_modes out_mode);
		static void set_direction(Spin::Controller::e_directions direction);
	};
};
#endif //__C_OUTPUT_H__
