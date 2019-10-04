/*
* c_output.cpp
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#include "c_output.h"

Spin::Output::s_pid_terms Spin::Output::as_position;
Spin::Output::s_pid_terms Spin::Output::as_velocity;
Spin::Output::s_pid_terms Spin::Output::as_torque;
Spin::Output::s_pid_terms * Spin::Output::active_pid_mode;
Spin::Controller::e_drive_modes Spin::Output::out_mode;


void Spin::Output::initialize()
{
	Spin::Output::set_pid_defaults();//<-- prep pid values for servo and velocity mode
	Spin::Output::setup_pwm_timer();//<--prep the pwm output timer
	Spin::Output::set_drive_state(Spin::Controller::e_drive_states::Disabled );//<--set the drive to disabled

	Spin::Controller::host_serial.print_string("output initialized\r");
	
	
}

void Spin::Output::set_pid_defaults()
{
	Spin::Output::as_position.reset();
	Spin::Output::as_position.invert_output = 1;
	Spin::Output::as_position.kP = 1;
	Spin::Output::as_position.kI = 0;
	Spin::Output::as_position.kD = 0;
	Spin::Output::as_position.max = 255;
	Spin::Output::as_position.min = 0;
	
	Spin::Output::as_velocity.reset();
	Spin::Output::as_velocity.invert_output = 1;
	Spin::Output::as_velocity.kP = 1;
	Spin::Output::as_velocity.kI = 1;
	Spin::Output::as_velocity.kD = 1;
	Spin::Output::as_velocity.max = 255;
	Spin::Output::as_velocity.min = 1;

	Spin::Output::as_torque.reset();
	Spin::Output::as_torque.invert_output = 1;
	Spin::Output::as_torque.kP = 1;
	Spin::Output::as_torque.kI = 0;
	Spin::Output::as_torque.kD = 0;
	Spin::Output::as_torque.max = 255;
	Spin::Output::as_torque.min = 0;
	
}

void Spin::Output::set_drive_state(Spin::Controller::e_drive_states state)
{
	if (state)
	{
		//enable pwm output
		DDRD |= (1<<PWM_OUTPUT_PIN);
		TCCR0B |= (1 << CS00);
	}
	else
	{
		//disable pwm output
		DDRD &= ~(1<<PWM_OUTPUT_PIN);
		TCCR0B = 0;
	}
}

void Spin::Output::set_output()
{
	OCR0A = Spin::Output::active_pid_mode->output;
}

void Spin::Output::setup_pwm_timer()
{
	OCR0A = 255;
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	
	return;
}

void Spin::Output::set_mode(Spin::Controller::e_drive_modes new_mode)
{

	Spin::Output::out_mode = new_mode;

	switch (new_mode)
	{
		case Spin::Controller::e_drive_modes::Velocity:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_velocity;//<--position mode (servo)
			break;
		}
		case Spin::Controller::e_drive_modes::Position:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_position;//<--position mode (servo)
			break;
		}
		case Spin::Controller::e_drive_modes::Torque:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_torque;//<--position mode (servo)
			break;
		}
		
		default:
		{
			Spin::Output::active_pid_mode = &Spin::Output::as_position;//<--position mode (servo)
			break;
		}
	}
	
}
