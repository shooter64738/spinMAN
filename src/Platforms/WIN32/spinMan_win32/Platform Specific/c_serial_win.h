/*
* c_serial_win.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/

//#include "..\..\..\..\Common\Serial\s_Buffer.h"
//#include "../../../../Serial/c_Serial.h"
//#include "..\..\..\..\hardware_def.h"
#include "../../../../hardware_def.h"
#include "../../../../Serial/s_buffer.h"

#ifdef MSVC

#ifndef __C_SERIAL_WIN_H__
#define __C_SERIAL_WIN_H__

namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:
		static s_Buffer rxBuffer[];
		protected:
		private:

		//functions
		public:
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static void send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char * data, uint8_t data_size);
		static void _add(uint8_t port, char byte, uint16_t position);

		static void disable_tx_isr();
		static void enable_tx_isr();
		protected:
		private:
	}; 
};
#endif //__C_SERIAL_WIN_H__
#endif