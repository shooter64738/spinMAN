/*
* c_output.h
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#ifndef __C_OUTPUT_H__
#define __C_OUTPUT_H__

#include "../hardware_def.h"
#include "../Serial/c_Serial.h"
#include <stdint.h>
#include <math.h>
#include "../core/c_enumerations.h"


#define MAX_LONG INT32_MAX
#define MAX_I_TERM (MAX_LONG / 2)
#define PID_SCALING_FACTOR 128

namespace Spin
{
	class Output
	{
	public:

		struct s_flags
		{
			uint32_t step_counter;
			Spin::Enums::e_drive_modes out_mode;
			Spin::Enums::e_drive_states enable;
			Spin::Enums::e_directions direction;
		};



		static s_flags Controls;

	protected:
	private:

	public:
		static void initialize();
		static void set_output(int32_t value);
		static void set_drive_state(Spin::Enums::e_drive_states state);
		static void set_mode(Spin::Enums::e_drive_modes out_mode);
		static void set_direction(Spin::Enums::e_directions direction);
	};
};
#endif //__C_OUTPUT_H__
