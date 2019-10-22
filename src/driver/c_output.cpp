/*
* c_output.cpp
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#include "c_output.h"
#include "../core/c_configuration.h"
#include "c_controller.h"
#include "../closed loop/c_pid.h"

Spin::Output::s_flags Spin::Output::Controls;


void Spin::Output::initialize()
{

	HardwareAbstractionLayer::Outputs::initialize();//<--prep the pwm output timer

	Spin::Driver::Controller::host_serial.print_string("output initialized\r\n");

}



void Spin::Output::set_drive_state(Spin::Enums::e_drive_states state)
{
	//If state is already set to same value, just return
	if (Spin::Output::Controls.enable == state)
	return;
	
	Spin::Output::Controls.enable = state;

	if (state == Enums::e_drive_states::Enabled)
	{
		HardwareAbstractionLayer::Outputs::enable_output();
	}
	else
	{
		HardwareAbstractionLayer::Outputs::disable_output();
	}
}

void Spin::Output::set_output(int32_t value)
{
	//Spin::Driver::Controller::host_serial.print_string(" pwm:");
	//Spin::Driver::Controller::host_serial.print_int32(Spin::Output::active_pid_mode->output);
	HardwareAbstractionLayer::Outputs::update_output(value);
}

void Spin::Output::set_mode(Spin::Enums::e_drive_modes new_mode)
{
	//If mode is already set to same value, just return
	if (Spin::Output::Controls.out_mode == new_mode)
	return;

	Spin::Output::Controls.out_mode = new_mode;

	switch (new_mode)
	{
		case Spin::Enums::e_drive_modes::Velocity:
		{
			Spin::ClosedLoop::Pid::active_factors = &Spin::Configuration::PID_Tuning.Velocity;
			Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Velocity);
			break;
		}
		case Spin::Enums::e_drive_modes::Position:
		{
			Spin::ClosedLoop::Pid::active_factors = &Spin::Configuration::PID_Tuning.Position;
			Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Position);
			break;
		}
		case Spin::Enums::e_drive_modes::Torque:
		{
			Spin::ClosedLoop::Pid::active_factors = &Spin::Configuration::PID_Tuning.Torque;
			Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Torque);
			break;
		}

		default:
		{
			Spin::ClosedLoop::Pid::active_factors = &Spin::Configuration::PID_Tuning.Velocity;
			Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Velocity);
			break;
		}
	}
}

void Spin::Output::set_direction(Spin::Enums::e_directions direction)
{
	//If direction is already set to same value, just return
	if (Spin::Output::Controls.direction == direction)
	return;
	
	Spin::Output::Controls.direction = direction;
	HardwareAbstractionLayer::Outputs::set_direction(direction);
}