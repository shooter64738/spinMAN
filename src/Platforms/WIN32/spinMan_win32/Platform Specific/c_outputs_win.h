#pragma once
#include <stdint.h>
#include "../../../../core/c_enumerations.h"
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
		static void set_direction(Spin::Enums::e_directions direction);
	};
};

