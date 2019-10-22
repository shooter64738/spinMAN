
/*
* c_pid.h
*
* Created: 10/9/2019 9:23:53 AM
* Author: jeff_d
*/


#ifndef __C_POSITION_H__
#define __C_POSITION_H__

#include <stdint.h>

namespace Spin
{
	namespace ClosedLoop
	{
		class Position
		{
			//variables
			public:


			protected:
			private:

			//functions
			public:
			static void step();
			static void step(int32_t target, int32_t actual);
			static bool _check_range(int32_t target_rpm);
			static bool _check_tolerance(int32_t target_rpm);
			static void _set_state();
			static int32_t _clamp_acceleration(int32_t target, int32_t actual);
			static void _find_closest_error(int32_t target, int32_t actual);
			protected:
			private:
		};
	};
};
#endif
