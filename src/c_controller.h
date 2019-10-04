/*
* c_controller.h
*
* Created: 10/3/2019 4:29:27 PM
* Author: jeff_d
*/


#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"
#include <stdint.h>


namespace Spin
{
	class Controller
	{
		//variables
		public:
		enum e_control_flags
		{
			Enable_Disbale = 0,
			Mode = 1,
			Direction = 2
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
			Reverse = 1
		};
		static c_Serial host_serial;
		static uint8_t pid_interval;
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void run();
		static void check_critical_states();
		static void process();

		static void check_pid_cycle();

		protected:
		private:
		

	}; //c_controller
};
#endif //__C_CONTROLLER_H__
