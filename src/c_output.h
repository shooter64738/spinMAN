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


#define MAX_LONG INT32_MAX
#define MAX_I_TERM (MAX_LONG / 2)
#define SCALING_FACTOR 64
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
			int16_t maxError;
			int32_t maxSumError;
			int32_t sumError;
			int16_t lastProcessValue;
			int32_t MAX_INT;
			void reset()
			{
				MAX_INT = INT16_MAX;
				kP = 0;
				kI = 0;
				kD = 0;
				proportional = 0;
				integral = 0;
				derivative = 0;
				preError = 0;
				sumError = 0;
				max = 0;
				min = 0;
				maxError = 0;
				maxSumError = 0;
				output = OUTPUT_OFF;
				invert_output = 0;
				lastProcessValue = 0;
			}
			
			void initialize()
			{
				kP = kP*SCALING_FACTOR;
				kI = kI*SCALING_FACTOR;
				kD = kD*SCALING_FACTOR;

				maxError = MAX_INT / (kP + 1);
				maxSumError = MAX_I_TERM / (kI + 1);
			}

			int32_t calc(int16_t setPoint, int16_t processValue)
			{

				
			}

			int32_t get_pid(int32_t setPoint, int32_t processValue)
			{
				int16_t errors, p_term, d_term;
				int32_t i_term, temp;

				errors = setPoint - processValue;

				// Calculate Pterm and limit error overflow
				if (errors > maxError) {
					p_term = MAX_INT;
				}
				else if (errors < -maxError) {
					p_term = -MAX_INT;
				}
				else {
					p_term = kP * errors;
				}

				// Calculate Iterm and limit integral runaway
				temp = sumError + errors;
				if (temp > maxSumError) {
					i_term = MAX_I_TERM;
					sumError = maxSumError;
				}
				else if (temp < -maxSumError) {
					i_term = -MAX_I_TERM;
					sumError = -maxSumError;
				}
				else {
					sumError = temp;
					i_term = kI * sumError;
				}

				// Calculate Dterm
				d_term = kD * (lastProcessValue - processValue);

				lastProcessValue = processValue;

				output = (p_term + i_term + d_term) / SCALING_FACTOR;
				if (output > MAX_INT) {
					output = MAX_INT;
				}
				else if (output < -MAX_INT) {
					output = -MAX_INT;
				}
				output >>= 7; //bit shift divide this by 128
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
