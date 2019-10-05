#ifndef __C_CORE_328_P_H__
#define __C_CORE_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

namespace HardwareAbstractionLayer
{
	class Core
	{
		public:
		static void initialize();
		static void start_interrupts();
		
	};
};
#endif