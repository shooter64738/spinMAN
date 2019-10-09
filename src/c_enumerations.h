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
		enum e_encoder_modes
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

		enum e_drive_modes
		{
			Velocity = 0,
			Position = 1,
			Torque = 2,
			Invalid = 3
		};

		enum e_drive_states
		{
			Disabled = 0,
			Enabled = 1
		};

		enum e_directions
		{
			Forward = 0,
			Reverse = 1,
			Free = 2
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
