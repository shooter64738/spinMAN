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
#include <math.h>
#include "c_enumerations.h"


#define MAX_LONG INT32_MAX
#define MAX_I_TERM (MAX_LONG / 2)
#define PID_SCALING_FACTOR 128

namespace Spin
{
	class Output
	{
	public:
		struct s_pid_returns
		{
			int32_t errors, p_term, d_term, i_term, output;
			int16_t lastProcessValue, lasterror;
			int8_t errors_direction;
		};


		struct s_pid_terms
		{
			int32_t kP, kI, kD, proportional, integral, derivative
			, maxSumError, sumError, preError, MAX_INT;
			int32_t max, min, maxError;
			uint8_t invert_output, resolution= 100;

			s_pid_returns pid_calc;
			Spin::Enums::e_directions control_direction;
			Spin::Enums::e_drive_modes control_mode;
			
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
				
				invert_output = 0;
				pid_calc.lastProcessValue = 0;
				pid_calc.lasterror = 0;
				pid_calc.errors = 0;
				pid_calc.p_term = 0;
				pid_calc.d_term = 0;
				pid_calc.i_term = 0;
				pid_calc.output = 0;
				pid_calc.errors_direction = 0;
			}

			void initialize()
			{
				maxError = MAX_INT / (kP + 1);
				maxSumError = MAX_I_TERM / (kI + 1);
			}

			void reset_integral()
			{
				sumError = 0;
			}

			void _calculate(int32_t setPoint, int32_t processValue, s_pid_returns *pid_calc)
			{

				int32_t temp;
				pid_calc->errors = setPoint - processValue;
				pid_calc->errors_direction = (pid_calc->errors < 0) ? -1 : 1;

				// Calculate Pterm and limit error overflow
				if (pid_calc->errors > maxError) {
					pid_calc->p_term = MAX_INT;
				}
				else if (pid_calc->errors < -maxError) {
					pid_calc->p_term = -MAX_INT;
				}
				else {
					pid_calc->p_term = kP * pid_calc->errors;
				}

				// Calculate Iterm and limit integral runaway
				temp = sumError + (pid_calc->errors);
				if (temp > maxSumError) {
					pid_calc->i_term = MAX_I_TERM;
					sumError = maxSumError;
				}
				else if (temp < -maxSumError) {
					pid_calc->i_term = -MAX_I_TERM;
					sumError = -maxSumError;
				}
				else {
					sumError = temp;
					pid_calc->i_term = (kI * sumError);
				}
				
				//sumError -= (lasterror-errors);
				pid_calc->lasterror = pid_calc->errors;
				// Calculate Dterm
				pid_calc->d_term = (kD * (pid_calc->lastProcessValue - processValue));

				pid_calc->lastProcessValue = processValue;



				pid_calc->output = ((pid_calc->p_term + pid_calc->i_term + pid_calc->d_term) / PID_SCALING_FACTOR);

			}

			int32_t _clamp_output(int32_t output)
			{

				//keep output value in range for 8bit numbers
				//if (output > UINT8_MAX)
				//{
				//	output = UINT8_MAX; //we have hit + saturation
				//	sumError -= pid_calc.errors; //added to prevent integral windup (dont accumulate error after saturation)
				//}
				//else if (output < -UINT8_MAX)
				//{
				//	output = -UINT8_MAX; //we have hit - saturation
				//	sumError -= pid_calc.errors;  //added to prevent integral windup (dont accumulate error after saturation)
				//}

				if (output > max)
				{
					output = max; //we have hit + saturation
					sumError -= pid_calc.errors; //added to prevent integral windup (dont accumulate error after saturation)
				}
				else if (output < min)
				{
					output = min; //we have hit - saturation
					sumError -= pid_calc.errors;  //added to prevent integral windup (dont accumulate error after saturation)
				}

				//if output is - we have passed our set point
				//if output is + we are approaching our set point

				//drop the sign from the output
				//output = abs(output);
				if (invert_output)
					output = max - output;

				return output;

			}

			int32_t get_pid(int32_t setPoint, int32_t processValue)
			{

				_calculate(setPoint, processValue, &pid_calc);
				
				//pid_calc.output >>= ((PWM_RESOLUTION_BIT - 1));
				pid_calc.output = _clamp_output(pid_calc.output);

				//if we are in velcotu mode the +/- values mean to speed up or slow down
				//if (control_mode == Spin::Controller::e_drive_modes::Position)
				//{
				//}
				//if we are in position mode the +/- values mean to change motor directions
				//if (control_mode == Spin::Controller::e_drive_modes::Position)
				{
					/*
					IF a direction change is made we must:
					1. Reset the integral to drain the windup.
					2. Recalculate the pid output value
					If we do not, the motor may be running WOT and suddenly we command 
					it to change direction. I am using very large DC motors and have
					had one of them break a clamp and jump into the floor.
					Not to mention the tremendous load placed on the H-Bridge driver. 
					*/
					//calculation shows we need to rotate motor backward. are we already going backward?
					//if (pid_calc.errors_direction < 0 && control_direction == Spin::Controller::e_directions::Forward)
					//{
					//
						////set motor direction to reverse
						//control_direction = Spin::Controller::e_directions::Reverse;
						//
						////if we have changed directions reset the integral, or we have to wait for it to unwind
						//reset_integral();
						////get a new value. basically as if we are starting from scratch
						//_calculate(setPoint, processValue, &pid_calc);
						//pid_calc.output = (pid_calc.output >> 7);
						//pid_calc.output = _clamp_output(pid_calc.output);
//
					//}
					////calculation shows we need to rotate motor forward. are we already going forward?
					//if (pid_calc.errors_direction > 0 && control_direction == Spin::Controller::e_directions::Reverse)
					//{
						////set motor direction to reverse
						//control_direction = Spin::Controller::e_directions::Forward;
						//
						////if we have changed directions reset the integral, or we have to wait for it to unwind
						//reset_integral();
						////get a new value. basically as if we are starting from scratch
						//_calculate(setPoint, processValue, &pid_calc);
						//pid_calc.output = (pid_calc.output >> 7);
						//pid_calc.output = _clamp_output(pid_calc.output);
						//
					//}
					
			
					//put the sign back on the pid value, just cause... 
					//pid_calc.output = pid_calc.output<0?pid_calc.output*1:pid_calc.output;
				}
				return pid_calc.output;
			}
		};

		struct s_flags
		{
			uint32_t step_counter;
			Spin::Enums::e_drive_modes out_mode;
			Spin::Enums::e_drive_states enable;
			Spin::Enums::e_directions direction;
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
		static void set_pid_values();
		static void set_drive_state(Spin::Enums::e_drive_states state);
		static void set_mode(Spin::Enums::e_drive_modes out_mode);
		static void set_direction(Spin::Enums::e_directions direction);
	};
};
#endif //__C_OUTPUT_H__
