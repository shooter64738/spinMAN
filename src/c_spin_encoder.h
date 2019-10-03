/*
* c_spin_encoder.h
*
* Created: 10/1/2019 7:58:55 PM
* Author: Family
*/


#ifndef __C_SPIN_INPUT_ENCODER_H__
#define __C_SPIN_INPUT_ENCODER_H__
#define encoder_ticks_per_rev 400

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
namespace Spin
{
	namespace Input
	{

		class Encoder
		{
			//variables
			public:
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void encoder_update();
			protected:
			private:

			
		};
	};
};
#endif
