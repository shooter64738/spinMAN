/*
* c_controller.cpp
*
* Created: 10/3/2019 4:29:26 PM
* Author: jeff_d
*/


#include "c_controller.h"
#include "c_input.h"
#include "c_output.h"
#include "c_configuration.h"

//#define DEBUG_AVR
//#define DEBUG_AVR_PID
//#define MOTOR_AVR_TUNER
#ifdef MSVC
#define DEBUG_WIN32
#endif

c_Serial Spin::Controller::host_serial;

uint8_t Spin::Controller::pid_interval = 0;
uint8_t Spin::Controller::one_interval = 0;

static Spin::Controller::e_drive_modes controller_mode = Spin::Controller::e_drive_modes::Velocity;

void Spin::Controller::initialize()
{
	Spin::Controller::host_serial = c_Serial(0, 115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Controller::host_serial.print_string("proto type\r\n");//<-- Send hello message

	//enable interrupts
	HardwareAbstractionLayer::Core::start_interrupts();

	Spin::Configuration::initiailize();
	Spin::Input::initialize();
	Spin::Output::initialize();
	Spin::Controller::check_critical_states();
	
}

void Spin::Controller::run()
{
	//HardwareAbstractionLayer::Outputs::enable_output();
	//HardwareAbstractionLayer::Outputs::update_output(200);
	//HardwareAbstractionLayer::Outputs::set_direction(1);
	//while(1)
	//{
		//
	//}
	
	
	#ifdef DEBUG_WIN32
	//!!!ONLY USE THESE DEFAULTS FOR PROGRAM TESTING!!!
	//This defaults the drive to enbled
	Spin::Input::Controls.enable = Spin::Controller::e_drive_states::Disabled;
	Spin::Input::Controls.in_mode = Spin::Controller::e_drive_modes::Position;
	//This defaults the mode to velocity
	HardwareAbstractionLayer::Inputs::start_wave_read();	
	#endif
	HardwareAbstractionLayer::Outputs::set_direction(1);

	while (1)
	{

		Spin::Controller::check_critical_states();//<--critical that we check this often
		HardwareAbstractionLayer::Inputs::synch_hardware_inputs();
		HardwareAbstractionLayer::Inputs::check_intervals(); //<--See which intervals have a time match
		if (Spin::Input::Controls.enable == Enabled)
		{
			Spin::Controller::check_pid_cycle();//<--check if pid time has expired, and update if needed
		}

		Spin::Controller::process();//<--General processing. Perhaps an LCD update
	}
}

void Spin::Controller::check_critical_states()
{
	if (Spin::Input::Controls.enable == Disabled)
	{
		/*if soft stops are configured (motor free spins to a stop)
		set both direction pins high and the motor can free wheel to
		a complete stop
		*/
		if (!Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable)
		{
			Spin::Output::set_direction(Free);
		}
		
		Spin::Output::set_drive_state(Disabled);
		return; //<--drive is disable, theres not really any point in going further.
	}
	
	//See if enable has changed
	if (Spin::Input::Controls.enable != Spin::Output::Controls.enable)
	{
		Spin::Controller::host_serial.print_string("DRIVE ENABLE:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.in_mode);

		//start/stop pwm output pin, and stop/start pwm signal
		Spin::Output::set_drive_state(Spin::Input::Controls.enable);
		//reset the pid variables on state change
		Spin::Output::set_pid_defaults();
		//set the output mode to the mode specified by input
		Spin::Output::set_mode(Spin::Input::Controls.in_mode);
		//set the direction specified by the input pins
		Spin::Output::set_direction(Spin::Input::Controls.direction);
	}

	//See if mode has changed
	if (Spin::Input::Controls.in_mode != Spin::Output::Controls.out_mode)
	{

		//set the output mode to the mode specified by input
		Spin::Output::set_mode(Spin::Input::Controls.in_mode);
	}

	//See if direction has changed, but NOT if we are in position mode
	if (Spin::Input::Controls.direction != Spin::Output::Controls.direction)
	{
		if (Spin::Input::Controls.in_mode != Controller::e_drive_modes::Position)
		{
			Spin::Controller::host_serial.print_string("DRIVE DIRECTION:");
			Spin::Controller::host_serial.print_int32(Spin::Input::Controls.in_mode);
			Spin::Output::set_direction(Spin::Input::Controls.direction);
		}
		//reset the pid variables on direction change
		//Spin::Output::set_pid_defaults();
	}
}

void Spin::Controller::process()
{
	if (Spin::Controller::one_interval) //<--one second interval for general purpose reporting
	{
		Spin::Controller::one_interval = 0;
		
		Spin::Controller::host_serial.print_string("mod:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.in_mode);
		Spin::Controller::host_serial.print_string(" ena:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.enable);
		Spin::Controller::host_serial.print_string(" rpm:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.sensed_rpm);
		Spin::Controller::host_serial.print_string(" pos:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.sensed_position);
		Spin::Controller::host_serial.print_string(" stp:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.step_counter);
		
		Spin::Controller::host_serial.print_string("\r\n");
		
	}
}

void Spin::Controller::check_pid_cycle()
{
	
	if (Spin::Controller::pid_interval) //<--is it time to calculate PID again?
	{
		
		Spin::Controller::pid_interval = 0;
		
		if (Spin::Output::active_pid_mode != NULL && Spin::Output::Controls.enable)
		{
			//calculate the change
			switch (Spin::Output::Controls.out_mode)
			{
				case Velocity:
				{
					Spin::Input::Controls.step_counter = 200;
					Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.step_counter, Spin::Input::Controls.sensed_rpm);
					break;
				}
				case Position:
				{
					
					//this is too touchy for PWM with interference. Hard setting a position value for testing
					Spin::Input::Controls.step_counter = 200;
					Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.step_counter, Spin::Input::Controls.sensed_position);
					//in position mode pid can return a + or - value.
					//the direction flag should already be set.
					if (Spin::Output::active_pid_mode->control_direction != Spin::Output::Controls.direction)
					{
						//On direction changes, we reset integral to dump the windup up.
						Spin::Output::set_direction(Spin::Output::active_pid_mode->control_direction);
					}
					
					break;
				}
				default:
				/* Your code here */
				break;
			}
			HardwareAbstractionLayer::Outputs::update_output(Spin::Output::active_pid_mode->output);
		}
		else
		{
			#ifdef DEBUG_AVR
			Spin::Controller::host_serial.print_string(" pid not active\r\n");
			#endif
		}
	}
}