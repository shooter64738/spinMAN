/*
* c_output.cpp
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#include "c_output.h"
#include "hardware_def.h"
#include "c_configuration.h"

Spin::Output::s_flags Spin::Output::Controls;

Spin::Output::s_pid_terms Spin::Output::as_position;
Spin::Output::s_pid_terms Spin::Output::as_velocity;
Spin::Output::s_pid_terms Spin::Output::as_torque;
Spin::Output::s_pid_terms * Spin::Output::active_pid_mode;
//Spin::Controller::e_drive_modes Spin::Output::out_mode;
//Spin::Controller::e_drive_states Spin::Output::enable;


void Spin::Output::initialize()
{
	Spin::Output::set_pid_defaults();//<-- prep pid values for servo and velocity mode
	HardwareAbstractionLayer::Outputs::initialize();//<--prep the pwm output timer
	
	Spin::Output::set_mode(Spin::Controller::e_drive_modes::Velocity);
	Spin::Output::set_drive_state(Spin::Controller::e_drive_states::Disabled);//<--set the drive to disabled

	Spin::Controller::host_serial.print_string("output initialized\r\n");


}

void Spin::Output::set_pid_defaults()
{
	Spin::Output::as_position.reset();
	Spin::Output::as_position.invert_output = Spin::Configuration::Drive_Settings.Drive_Output_Inverted;
	Spin::Output::as_position.kP = Spin::Configuration::PID_Tuning.Position.Kp * PID_SCALING_FACTOR;
	Spin::Output::as_position.kI = Spin::Configuration::PID_Tuning.Position.Ki * PID_SCALING_FACTOR;
	Spin::Output::as_position.kD = Spin::Configuration::PID_Tuning.Position.Kd * PID_SCALING_FACTOR;
	Spin::Output::as_position.max = 239;	//<--this needs to be the first value that causes the motor
											//to turn +1. 255 is minimal output on 8bit pwm, but 254
											//may not cause the motor to start. IF this is not set
											//properly you may experience what appears to be pid oscillation
											//but is really just motor dead time. 
	Spin::Output::as_position.min = -239;
	Spin::Output::as_position.initialize();
	Spin::Output::as_position.control_mode = Spin::Controller::e_drive_modes::Position;
	Spin::Output::as_position.resolution = Spin::Configuration::PID_Tuning.Position.Allowed_Error_Percent;

	Spin::Output::as_velocity.reset();
	Spin::Output::as_velocity.invert_output = Spin::Configuration::Drive_Settings.Drive_Output_Inverted;
	Spin::Output::as_velocity.kP = Spin::Configuration::PID_Tuning.Velocity.Kp * PID_SCALING_FACTOR;
	Spin::Output::as_velocity.kI = Spin::Configuration::PID_Tuning.Velocity.Ki * PID_SCALING_FACTOR;
	Spin::Output::as_velocity.kD = Spin::Configuration::PID_Tuning.Velocity.Kd * PID_SCALING_FACTOR;
	Spin::Output::as_velocity.max = 255;
	Spin::Output::as_velocity.min = 0;
	Spin::Output::as_velocity.initialize();
	Spin::Output::as_velocity.control_mode = Spin::Controller::e_drive_modes::Velocity;
	Spin::Output::as_velocity.resolution = Spin::Configuration::PID_Tuning.Velocity.Allowed_Error_Percent;

	Spin::Output::as_torque.reset();
	Spin::Output::as_torque.invert_output = Spin::Configuration::Drive_Settings.Drive_Output_Inverted;
	Spin::Output::as_torque.kP = Spin::Configuration::PID_Tuning.Torque.Kp * PID_SCALING_FACTOR;
	Spin::Output::as_torque.kI = Spin::Configuration::PID_Tuning.Torque.Ki * PID_SCALING_FACTOR;
	Spin::Output::as_torque.kD = Spin::Configuration::PID_Tuning.Torque.Kd * PID_SCALING_FACTOR;;
	Spin::Output::as_torque.max = 255;
	Spin::Output::as_torque.min = 0;
	Spin::Output::as_torque.initialize();
	Spin::Output::as_torque.control_mode = Spin::Controller::e_drive_modes::Torque;
	Spin::Output::as_torque.resolution = Spin::Configuration::PID_Tuning.Torque.Allowed_Error_Percent;

}

void Spin::Output::set_drive_state(Spin::Controller::e_drive_states state)
{
	Spin::Output::Controls.enable = state;

	if (state == Controller::Enabled)
	{
		HardwareAbstractionLayer::Outputs::enable_output();
	}
	else
	{
		HardwareAbstractionLayer::Outputs::disable_output(OUTPUT_OFF);
		if (Spin::Output::active_pid_mode != NULL)
			Spin::Output::active_pid_mode->output = OUTPUT_OFF;
	}
}

void Spin::Output::set_output()
{
	//Spin::Controller::host_serial.print_string(" pwm:");
	//Spin::Controller::host_serial.print_int32(Spin::Output::active_pid_mode->output);
	HardwareAbstractionLayer::Outputs::update_output(abs(Spin::Output::active_pid_mode->output));
}

void Spin::Output::set_mode(Spin::Controller::e_drive_modes new_mode)
{

	Spin::Output::Controls.out_mode = new_mode;

	switch (new_mode)
	{
		case Spin::Controller::e_drive_modes::Velocity:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_velocity;//<--velocity mode (spindle)
			break;
		}
		case Spin::Controller::e_drive_modes::Position:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_position;//<--position mode (servo)
			break;
		}
		case Spin::Controller::e_drive_modes::Torque:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_torque;//<--position mode (spindle/servo)
			break;
		}

		default:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_position;//<--position mode (servo)
			break;
		}
	}

}

void Spin::Output::set_direction(Spin::Controller::e_directions direction)
{
	Spin::Output::Controls.direction = direction;
	HardwareAbstractionLayer::Outputs::set_direction(direction);
}