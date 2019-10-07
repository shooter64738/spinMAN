#pragma once
#include <thread>
namespace HardwareAbstractionLayer
{
	class Inputs
	{
	public:
		
		
		static void get_rpm();
		static void get_set_point();
		static void initialize();
		static void configure();
		static void update_encoder();
		static void synch_hardware_inputs();
		static void start_wave_read();
		static void HardwareAbstractionLayer::Inputs::timer2_compa_vect_thread();
	private:
		static std::thread timer2_comp_a_thread;
	};
};

