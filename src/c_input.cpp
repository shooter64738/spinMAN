/*
* c_input.cpp
*
* Created: 10/3/2019 4:14:44 PM
* Author: jeff_d
*/


#include "c_input.h"
#include "hardware_def.h"


Spin::Input::s_flags Spin::Input::Controls;

volatile uint8_t control_old_states = 255;
volatile uint8_t step_old_states = 255;




//static uint8_t rpm_slot = 0;



void Spin::Input::check_input_states()
{
	
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs();
	
	Spin::Controller::check_critical_states();
}


void Spin::Input::initialize()
{
	Spin::Input::setup_pulse_inputs();
	Spin::Input::check_input_states();
	
	Spin::Controller::host_serial.print_string("input initialized\r\n");//<-- Send hello message
}

void Spin::Input::setup_pulse_inputs()
{

	HardwareAbstractionLayer::Inputs::configure();
	
	Spin::Input::timer_re_start();
}

void Spin::Input::timer_re_start()
{
}



