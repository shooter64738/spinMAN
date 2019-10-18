#ifndef __C_STORAGE_WIN_H__
#define __C_STORAGE_WIN_H__
#include <stdint.h>

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