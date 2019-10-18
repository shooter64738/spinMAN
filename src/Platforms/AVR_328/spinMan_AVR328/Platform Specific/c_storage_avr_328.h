#ifndef __C_EEPROM_328_P_H__
#define __C_EEPROM_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

namespace HardwareAbstractionLayer
{
	class Storage
	{
		public:
		static void initialize();
		static void load(char* data, uint16_t size);
		static void save(char* data, uint16_t size);
		
	};
};
#endif