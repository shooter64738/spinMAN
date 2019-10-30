
/*
* c_pid.h
*
* Created: 10/9/2019 9:23:53 AM
* Author: jeff_d
*/


#ifndef __C_PID_H__
#define __C_PID_H__

#include <stdint.h>
#include "../core/c_configuration.h"

#define MAX_LONG INT32_MAX
#define MAX_I_TERM (MAX_LONG / 2)
#define MAX_P_TERM (INT32_MAX / 2)
#define PID_SCALING_FACTOR 128
#define PID_MIN 0
#define PID_MAX 65535

namespace Spin
{
	namespace ClosedLoop
	{
		class Pid
		{
			//variables
			public:
			struct s_errors
			{
				int32_t process;
				int32_t previous;
				int32_t accumulated;
				int32_t max_process;
				int32_t max_accumulated;
				int8_t direction;

			};

			static s_errors errors;
			static int32_t output;
			static int32_t raw_output;

			static Spin::Configuration::s_pid_factors *active_factors;
			//static float scaler;
			protected:
			private:

			//functions
			public:
			static void Set_Factors(Spin::Configuration::s_pid_factors factors);
			static void Load_Factors_For_Mode(Spin::Enums::e_drive_modes Mode);
			static void Reset_integral();
			static void Calculate(int32_t processValue);
			static void Calculate(int32_t setPoint, int32_t processValue);
			static void Reset();
			static void Restart();

			
			
			protected:

			private:
				static void _internal_pid_comp(int32_t processValue);
				static void _internal_error_comp(int32_t setPoint, int32_t processValue);
				static void _clamp_output();
			static void _set_p_term();
			static void _set_i_term();
			static void _set_d_term(int32_t current_process_value);
		}; //c_pid
	};
};
#endif //__C_PID_H__
