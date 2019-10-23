#ifndef __C_CORE_SAM_D21_H__
#define __C_CORE_SAM_D21_H__

#include "sam.h"

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