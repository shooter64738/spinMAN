/*
* c_controller.h
*
* Created: 10/3/2019 4:29:27 PM
* Author: jeff_d
*/


#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__

#include "hardware_def.h"
#include "Serial\c_Serial.h"
#include <stdint.h>

namespace Spin
{
	class Controller
	{
		//variables
		public:
		//enum e_control_flags
		//{
			//Enable_Disbale = 0,
			//Mode = 1,
			//Direction = 2
		//};
		
		
		static c_Serial host_serial;
		static uint8_t pid_interval;
		static uint8_t one_interval;
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void calibrate();
		static void run();
		static void sync_out_in_control();
		static void process();
		static void check_pid_cycle();
		protected:
		private:
		

	}; //c_controller
};
#endif //__C_CONTROLLER_H__
