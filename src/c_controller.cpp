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
	HardwareAbstractionLayer::Core::start_interrupts();

	//!!!ONLY USE THESE DEFAULTS FOR PROGRAM TESTING!!!
	//This defaults the drive to enbled
	Spin::Input::Controls.enable = Spin::Controller::e_drive_states::Enabled;
	//This defaults the mode to velocity
	Spin::Input::Controls.in_mode = Spin::Controller::e_drive_modes::Velocity;

	Spin::Input::initialize();
	Spin::Output::initialize();
	Spin::Controller::check_critical_states();

}

void Spin::Controller::run()
{
	
	//OCR0A = 170;
	//TCCR0A |= (1 << COM0A1);
	//TCCR0A |= (1 << WGM01) | (1 << WGM00);
	////enable pwm output
	//DDRD |= (1<<PWM_OUTPUT_PIN);
	//TCCR0B |= (1 << CS00);
	//while (1)
	//{
	//
	//}
	
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
	//See if enable has changed
	if (Spin::Input::Controls.enable != Spin::Output::Controls.enable)
	{
		
			//start/stop pwm output pin, and stop/start pwm signal
			Spin::Output::set_drive_state(Spin::Input::Controls.enable);
			//reset the pid variables on state change
			Spin::Output::set_pid_defaults();
			//set the output mode to the mode specified by input
			Spin::Output::set_mode(Spin::Input::Controls.in_mode);
	}

	//See if mode has changed
	if (Spin::Input::Controls.in_mode != Spin::Output::Controls.out_mode)
	{
		Spin::Controller::host_serial.print_string(" MODE CHANGE\r");
		//reset the pid variables on mode change
		//Spin::Output::set_pid_defaults();
		//set the output mode to the mode specified by input
		//Spin::Output::set_mode(Spin::Input::Controls.in_mode);
	}
	
	//See if direction has changed
	if (Spin::Input::Controls.direction != Spin::Output::Controls.direction)
	{
		Spin::Controller::host_serial.print_string(" DIRECTION CHANGE\r");
		Spin::Output::set_direction(Spin::Input::Controls.direction);
		//reset the pid variables on direction change
		//Spin::Output::set_pid_defaults();
	}
}

void Spin::Controller::process()
{

	
}

void Spin::Controller::check_pid_cycle()
{

	if (Spin::Controller::pid_interval) //<--is it time to calculate PID again?
	{
		Spin::Controller::pid_interval = 0;
		Spin::Controller::host_serial.print_string("mod:");
		Spin::Controller::host_serial.print_int32(Spin::Output::Controls.out_mode);
		Spin::Controller::host_serial.print_string(" ena:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.enable);
		Spin::Controller::host_serial.print_string(" dir:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.direction);
		Spin::Controller::host_serial.print_string(" pid:");
		Spin::Controller::host_serial.print_int32(Spin::Output::active_pid_mode->output);
		
		if (Spin::Output::active_pid_mode != NULL && Spin::Input::Controls.enable)
		{
			
			
			//calculate the change
			Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.step_counter,Spin::Input::Controls.sensed_rpm);
			Spin::Output::set_output();
			
			
			
			
			Spin::Controller::host_serial.print_string(" rpm:");
			Spin::Controller::host_serial.print_int32(Spin::Input::Controls.sensed_rpm);
			Spin::Controller::host_serial.print_string(" set:");
			Spin::Controller::host_serial.print_int32(Spin::Input::Controls.step_counter);
			Spin::Controller::host_serial.print_string("\r");
		}
		else
		{
			Spin::Controller::host_serial.print_string(" pid not active\r");
		}
	}
}
