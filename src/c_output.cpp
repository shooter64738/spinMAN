/*
* c_output.cpp
*
* Created: 10/3/2019 4:17:13 PM
* Author: jeff_d
*/


#include "c_output.h"
#include "c_controller.h"

Spin::Control::Output::s_pid_terms Spin::Control::Output::as_position;
Spin::Control::Output::s_pid_terms Spin::Control::Output::as_velocity;
Spin::Control::Output::s_pid_terms * Spin::Control::Output::active_pid_mode;


void Spin::Control::Output::initialize()
{
	Spin::Control::Output::set_pid_defaults();//<-- prep pid values for servo and velocity mode
	Spin::Control::Output::setup_pwm_timer();//<--prep the pwm output timer
	Spin::Control::Output::set_drive_state(0);//<--set the drive to disabled

	Spin::Controller::host_serial.print_string("output initialized\r");
	
	
}

void Spin::Control::Output::set_pid_defaults()
{
	Spin::Control::Output::as_position.reset();
	Spin::Control::Output::as_position.kP = 1;
	Spin::Control::Output::as_position.kI = 0;
	Spin::Control::Output::as_position.kD = 0;
	Spin::Control::Output::as_position.max = 255;
	Spin::Control::Output::as_position.min = 0;
	
	Spin::Control::Output::as_velocity.reset();
	Spin::Control::Output::as_velocity.kP = 1;
	Spin::Control::Output::as_velocity.kI = 1;
	Spin::Control::Output::as_velocity.kD = 1;
	Spin::Control::Output::as_velocity.max = 255;
	Spin::Control::Output::as_velocity.min = 1;
	
}

void Spin::Control::Output::set_drive_state(uint8_t state)
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

float Spin::Control::Output::update_pid(uint32_t target, uint32_t current)
{
	Spin::Control::Output::active_pid_mode->get_pid(target,current);

}

void Spin::Control::Output::setup_pwm_timer()
{
	OCR0A = 255;
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	
	return;
}

void Spin::Control::Output::set_mode(uint8_t mode)
{
	if (mode == 1)
	Spin::Control::Output::active_pid_mode = &Spin::Control::Output::as_position;//<--position mode (servo)
	else
	Spin::Control::Output::active_pid_mode = &Spin::Control::Output::as_velocity;//<--velocity mode (spindle)
}
