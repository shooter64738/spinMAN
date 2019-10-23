
/*
* c_serial_avr_328.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/
#ifdef __SAMD21G18AU__

#ifndef __C_SERIAL_SAM_D21_H__
#define __C_SERIAL_SAM_D21_H__

#include "../../../../Serial/c_Serial.h"
#include "../../../../hardware_def.h"
#include "c_core_sam_d21.h"
#define clockDivisor(baud) F_CPU/(16*baud)

typedef enum _InterruptPriority{
	PRIOR_SERIAL = 3u, //Highest priority
	//PRIOR_SPI = 1u,
	//PRIOR_I2C = 2u,
	//PRIOR_TIMER = 3u
	
} InterruptPriority;

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
		static void _incoming_serial_isr(uint8_t Port, char Byte);
		static void _outgoing_serial_isr(uint8_t Port, char Byte);
		protected:
		private:
		//c_serial_avr_328( const c_serial_avr_328 &c );
		//c_serial_avr_328& operator=( const c_serial_avr_328 &c );
		//c_serial_avr_328();
		//~c_serial_avr_328();

	}; //c_serial_avr_328
};
#endif //__C_SERIAL_AVR_328_H__
#endif