#pragma once
#include <stdint.h>
namespace HardwareAbstractionLayer
{
	class Outputs
	{
	public:
		static void initialize();
		static void configure();
		static void enable_output();
		static void disable_output();
		static void disable_output(uint16_t off_value);
		static void update_output(uint16_t value);
	};
};

