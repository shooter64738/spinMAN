#pragma once
namespace HardwareAbstractionLayer
{
	class Inputs
	{
	public:
		static void get_rpm();
		static void initialize();
		static void configure();
		static void update_encoder();
		static void synch_hardware_inputs();
	};
};

