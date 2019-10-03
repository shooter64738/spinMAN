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
		static c_Serial host_serial;	
		
		static uint8_t pid_interval;
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void run();
		protected:
		private:
		

	}; //c_controller
};
#endif //__C_CONTROLLER_H__
