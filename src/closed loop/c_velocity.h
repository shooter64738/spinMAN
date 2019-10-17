
/*
* c_pid.h
*
* Created: 10/9/2019 9:23:53 AM
* Author: jeff_d
*/


#ifndef __C_VELOCITY_H__
#define __C_VELOCITY_H__

#include <stdint.h>

namespace Spin
{
	namespace ClosedLoop
	{
		class Velocity
		{
			//variables
			public:
			protected:
			private:

			//functions
			public:
			static void step(int32_t target, int32_t actual);
			
			protected:
			private:
				static bool _check_range(int32_t target_rpm);
				static bool _check_tolerance(int32_t target_rpm);
			
		}; //c_pid
	};
};
#endif //__C_PID_H__
