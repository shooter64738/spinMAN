/*
* c_controller.cpp
*
* Created: 10/3/2019 4:29:26 PM
* Author: jeff_d
*/


#include "c_controller.h"
#include "c_input.h"
#include "c_output.h"

#define SERVO_MODE 1
#define VELOCITY_MODE 0

c_Serial Spin::Controller::host_serial;

uint8_t Spin::Controller::pid_interval = 0;
static Spin::Controller::e_drive_modes controller_mode = Spin::Controller::e_drive_modes::Velocity;

void Spin::Controller::initialize()
{
	Spin::Controller::host_serial = c_Serial(0,115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Controller::host_serial.print_string("proto type\r");//<-- Send hello message
	//enable interrupts
	sei();

	//!!!ONLY USE THESE DEFAULTS FOR PROGRAM TESTING!!!
	//This defaults the drive to enbled
	Spin::Input::Controls.enable = Spin::Controller::e_drive_states::Enabled;
	//This defaults the mode to velocity
	Spin::Input::Controls.in_mode = Spin::Controller::e_drive_modes::Velocity;

	Spin::Input::initialize();
	Spin::Output::initialize();

}

void Spin::Controller::run()
{
	
	
	static int32_t new_pid = 255;
	/*
	#1 action priority is the enable pin. If that pin goes low EVER
	//we need to shut the drive down immediately.
	*/
	while(1)
	{
	
		Spin::Controller::check_critical_states();//<--critical that we check this often
		Spin::Input::update_rpm();//<--update rpm as often as we can
		Spin::Controller::check_critical_states();//<--critical that we check this often
		Spin::Input::update_time_keeping(); //<--if matched time, set pid flag, reset pid counter.
		Spin::Controller::check_critical_states();//<--critical that we check this often
		Spin::Controller::check_pid_cycle();//<--check if pid time has expired, and update if needed
		Spin::Controller::check_critical_states();//<--critical that we check this often
		
		Spin::Controller::process();
		
		
		
	}
}
void Spin::Controller::check_critical_states()
{
	Spin::Input::check_input_states();
	//See if drive is disabled
	if (!Spin::Input::Controls.enable)//<--drive is disabled
	{
		//stop pwm output pin, and stop pwm signal
		Spin:Output::set_drive_state(Spin::Controller::e_drive_states::Disabled);
		//reset the pid variables
		Spin::Output::set_pid_defaults();
	}

	//See if mode has changed
	if (Spin::Input::Controls.in_mode != Spin::Output::out_mode)//<--drive is disabled
	{
		//set the output mode to the mode specified by input
		Spin::Output::set_mode(Spin::Input::Controls.in_mode);
	}
}

void Spin::Controller::process()
{
	

	

	if (Spin::Input::Controls.direction == Spin::Controller::e_directions::Forward)
	{
		
	}
}

void Spin::Controller::check_pid_cycle()
{
	//If drive is disabled this is point less.
	if (!Spin::Input::Controls.enable)
	{
		return ;
	}

	if (Spin::Controller::pid_interval) //<--is it time to calculate PID again?
	{
		Spin::Controller::pid_interval = 0;
		//calculate the change
		Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.step_counter,Spin::Input::Controls.sensed_rpm);
		
		Spin::Controller::host_serial.print_string("pid:");
		Spin::Controller::host_serial.print_int32(Spin::Output::active_pid_mode->output);
		Spin::Controller::host_serial.print_string(" rpm:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.sensed_rpm);
		Spin::Controller::host_serial.print_string(" set:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.step_counter);
		Spin::Controller::host_serial.print_string("\r");
		
	}
}
