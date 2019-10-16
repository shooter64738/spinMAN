
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
			static int32_t output;

			protected:
			private:

			//functions
			public:
			static void Set_Factors(Spin::Configuration::s_pid_factors factors);
			static void Reset_integral();
			static void Calculate(int32_t setPoint, int32_t processValue);
			static void Reset();
			protected:
			
			private:
			static void _clamp_output();
			static void _set_p_term();
			static void _set_i_term();
			static void _set_d_term();
		}; //c_pid
	};
};
#endif //__C_PID_H__
