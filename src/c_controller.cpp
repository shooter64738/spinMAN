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

void Spin::Controller::initialize()
{
	Spin::Controller::host_serial = c_Serial(0,115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Controller::host_serial.print_string("proto type\r");//<-- Send hello message
	//enable interrupts
	sei();

	Spin::Input::initialize();
	Spin::Output::initialize();

}

void Spin::Controller::run()
{
	
	
	Spin::Input::Actions.Enable.Value = 1;
	Spin::Input::Actions.Enable.Dirty = 1;
	
	Spin::Input::Actions.Mode.Value = VELOCITY_MODE;
	Spin::Input::Actions.Mode.Dirty = 1;
	
	static int32_t new_pid = 255;
	
	while(1)
	{
		Spin::Input::update_rpm();//<--update rpm as often as we can

		if (Spin::Input::Actions.Enable.Dirty == 1) //<--has enable changed?
		{
			Spin::Output::set_drive_state(Spin::Input::Actions.Enable.Value);
			Spin::Controller::host_serial.print_string("en dirty\r");
			Spin::Controller::host_serial.print_string("DRV ");
			Spin::Controller::host_serial.print_string(Spin::Input::Actions.Enable.Value?"ENABLE\r":"DISABLE\r");
			Spin::Input::Actions.Enable.Dirty = 0;
			Spin::Output::set_pid_defaults();
		}
		if (Spin::Input::Actions.Rpm.Dirty == 1)  //<--has rpm changed?
		{
			Spin::Input::Actions.Rpm.Dirty = 0;
		}
		if (Spin::Input::Actions.Direction.Dirty == 1) //<--has direction changed?
		{
			Spin::Controller::host_serial.print_string("dr dirty\r");
			Spin::Input::Actions.Direction.Dirty = 0;
		}

		if (Spin::Input::Actions.Mode.Dirty == 1)  //<--has mode changed?
		{
			Spin::Output::set_mode(Spin::Input::Actions.Mode.Value);

			Spin::Controller::host_serial.print_string("md dirty\r");
			Spin::Input::Actions.Mode.Dirty = 0;
		}
		
		if (Spin::Input::Actions.Enable.Value)
		{
			if (Spin::Controller::pid_interval) //<--is it time to calcualte PID again?
			{
				pid_interval = 0;
				//update_pid(Spin::Input::Controls::Control.Step.Value, Spin::Input::Controls::Control.Rpm.Value);
				
				{
					new_pid = Spin::Output::active_pid_mode->get_pid
					(Spin::Input::Actions.Step.Value,Spin::Input::Actions.Rpm.Value)+1;		
					//(500,100);
					Spin::Controller::host_serial.print_string("pid:");
					Spin::Controller::host_serial.print_int32(new_pid);
					Spin::Controller::host_serial.print_string(" rpm:");
					Spin::Controller::host_serial.print_int32(Spin::Input::Actions.Rpm.Value);
					Spin::Controller::host_serial.print_string(" set:");
					Spin::Controller::host_serial.print_int32(Spin::Input::Actions.Step.Value);
					Spin::Controller::host_serial.print_string("\r");
				}
				OCR0A = new_pid;
			}
			if (Spin::Input::Actions.Step.Dirty == 1) //<--Has step changed?
			{
				//this is only here for testing. I need a 50khz input to the motor
				//drive and I am also borrowing that pulse generator as an rpm request
				//Spin::Input::Controls::Control.Step.Value = Spin::Input::Controls::Control.Step.Value/100;
				//Spin::Control::Output::update_pid(Spin::Control::Input::Actions.Step.Value, Spin::Control::Input::Actions.Rpm.Value);
				
				//Spin::Controller::host_serial.print_string("requested rpm:");
				//Spin::Controller::host_serial.print_int32(Spin::Control::Input::Actions.Step.Value);
				//Spin::Controller::host_serial.print_string("\r");
				Spin::Input::Actions.Step.Dirty = 0;
				Spin::Input::timer_re_start();
				
				
				//Write the output in every loop.
				//OCR0A = Spin::Control::Output::active_pid_mode->Max_Val -new_pid;
				//Spin::Controller::host_serial.print_string("\r");
			}
		}
		
		
	}
}