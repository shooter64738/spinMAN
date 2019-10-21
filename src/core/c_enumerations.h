/*
* c_enumerations.h
*
* Created: 10/9/2019 8:46:52 AM
* Author: jeff_d
*/


#ifndef __C_ENUMERATIONS_H__
#define __C_ENUMERATIONS_H__

namespace Spin
{
	namespace Enums
	{
		//variables
		//public:
		enum class e_config_results
		{
			Incomplete_Config = 0,
			Configuration_Ok = 1,
			Encoder_Not_Available = 2,
			Encoder_Available = 3,
			No_Encoder_PPR_Set = 4
		};

		enum class e_velocity_states
		{
			Invalid = 0,
			Accelerate = 1,
			Cruise = 2,
			Decelerate = 3,
			Stopping = 4
			
		};

		enum class e_position_states
		{
			Invalid = 0,
			Accel_To_Target = 1,
			On_Target = 2,
			Decel_To_Target = 3,
			Target_Exceeded = 4

		};
		
		enum class e_encoder_modes
		{
			No_Encoder = 0,
			Simple_ChanA = 1,
			Simple_ChanB = 2,
			Quadrature = 3,
			Index_Only = 4,
			Simple_ChanA_wIndex = 5,
			Simple_ChanB_wIndex = 6,
			Quadrature_wIndex = 7
		};

		enum class e_drive_modes
		{
			Invalid = 0,
			Velocity = 1,
			Position = 2,
			Torque = 3,
		};

		enum class e_drive_states
		{
			Invalid = 0,
			Disabled = 1,
			Enabled = 2
		};

		enum class e_directions
		{
			Invalid = 0,
			Forward = 1,
			Reverse = 2,
			Free = 3
		};

		//protected:
		//private:
//
		////functions
		//public:
		//protected:
		//private:
	}; //c_enumerations
};
#endif //__C_ENUMERATIONS_H__
