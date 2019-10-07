/*
* c_configuration.h
*
* Created: 10/6/2019 7:01:55 PM
* Author: Family
*/


#ifndef __C_CONFIGURATION_H__
#define __C_CONFIGURATION_H__

#include <stdint.h>

namespace Spin
{
	class Configuration
	{
		//variables
		public:
		struct s_pid_factors
		{
			int32_t Kp;
			int32_t Ki;
			int32_t Kd;
			int16_t Allowed_Error_Percent;
		};
		struct s_pid_tunings
		{
			s_pid_factors Position;
			s_pid_factors Velocity;
			s_pid_factors Torque;
			
		};

		struct s_drive_settings
		{
			uint8_t Hard_Stop_On_Disable;
			uint8_t Soft_Direction_Changes;
			uint8_t Drive_Output_Inverted;
		};
		static s_pid_tunings PID_Tuning;
		static s_drive_settings Drive_Settings;

		protected:
		private:

		//functions
		public:
		static void initiailize();
		static void load();
		static void save();
		static void modify();
		protected:
		private:

	}; //c_configuration
};
#endif //__C_CONFIGURATION_H__
