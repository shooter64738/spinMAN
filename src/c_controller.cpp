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

	Spin::Control::Input::initialize();
	Spin::Control::Output::initialize();

}

void Spin::Controller::run()
{
	
	
	Spin::Control::Input::Actions.Enable.Value = 1;
	Spin::Control::Input::Actions.Enable.Dirty = 1;
	
	Spin::Control::Input::Actions.Mode.Value = VELOCITY_MODE;
	Spin::Control::Input::Actions.Mode.Dirty = 1;
	//Spin::Control::Output::active_pid_mode = & Spin::Control::Output::as_velocity;

	
	while(1)
	{
		Spin::Control::Input::update_rpm();//<--update rpm as often as we can

		if (Spin::Control::Input::Actions.Enable.Dirty == 1) //<--has enable changed?
		{
			Spin::Control::Output::set_drive_state(Spin::Control::Input::Actions.Enable.Value);
			Spin::Controller::host_serial.print_string("en dirty\r");
			Spin::Control::Input::Actions.Enable.Dirty = 0;
		}
		if (Spin::Control::Input::Actions.Rpm.Dirty == 1)  //<--has rpm changed?
		{
			//Spin::Input::Controls::host_serial.print_string("rpm dirty\r");
			Spin::Control::Input::Actions.Rpm.Dirty = 0;
			//Spin::Input::Controls::update_vitals();
			Spin::Controller::host_serial.print_string("reported rpm: ");
			Spin::Controller::host_serial.print_float(Spin::Control::Input::Actions.Rpm.Value);
			Spin::Controller::host_serial.print_string("\r");
			//count in 1 second/400*60 = rpm
			//time/1000 *400*60
			//
		}
		if (Spin::Control::Input::Actions.Direction.Dirty == 1) //<--has direction changed?
		{
			Spin::Controller::host_serial.print_string("dr dirty\r");
			Spin::Control::Input::Actions.Direction.Dirty = 0;
		}

		if (Spin::Control::Input::Actions.Mode.Dirty == 1)  //<--has mode changed?
		{
			Spin::Control::Output::set_mode(Spin::Control::Input::Actions.Mode.Value);

			Spin::Controller::host_serial.print_string("md dirty\r");
			Spin::Control::Input::Actions.Mode.Dirty = 0;
		}
		
		if (Spin::Controller::pid_interval) //<--is it time to calcualte PID again?
		{
			pid_interval = 0;
			//update_pid(Spin::Input::Controls::Control.Step.Value, Spin::Input::Controls::Control.Rpm.Value);
			
			{
				
				//
				//Spin::Input::Controls::host_serial.print_string("read rpm:");Spin::Input::Controls::host_serial.print_float(Spin::Input::Controls::Control.Rpm.Value);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("control rpm:");Spin::Input::Controls::host_serial.print_float(Spin::Input::Controls::Control.Step.Value);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Kp:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Kp);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Ki:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Ki);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Kd");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Kd);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("cv_control_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->cv_control_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("pv_process_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->pv_process_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("p_proportional_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->p_proportional_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("i_integral_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->i_integral_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("d_derivative_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->d_derivative_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("e_error_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->e_error_last);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("e_error:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->e_error);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("set_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->actual_last);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("actual_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->set_last);Spin::Input::Controls::host_serial.print_string("\r");
				//while(1){}
			}
			//OCR0A = 200;// active_pid_mode->Max_Val -abs(active_pid_mode->cv_control_variable);
		}
		if (Spin::Control::Input::Actions.Step.Dirty == 1) //<--Has step changed?
		{
			//this is only here for testing. I need a 50khz input to the motor
			//drive and I am also borrowing that pulse generator as an rpm request
			//Spin::Input::Controls::Control.Step.Value = Spin::Input::Controls::Control.Step.Value/100;
			Spin::Control::Output::update_pid(Spin::Control::Input::Actions.Step.Value, Spin::Control::Input::Actions.Rpm.Value);
			
			Spin::Controller::host_serial.print_string("requested rpm:"); 
			Spin::Controller::host_serial.print_int32(Spin::Control::Input::Actions.Step.Value);
			Spin::Controller::host_serial.print_string("\r");
			Spin::Control::Input::Actions.Step.Dirty = 0;
			Spin::Control::Input::timer_re_start();
			
			Spin::Controller::host_serial.print_string("new pid value:");
			Spin::Controller::host_serial.print_float(Spin::Control::Output::active_pid_mode->Max_Val-abs(Spin::Control::Output::active_pid_mode->cv_control_variable));
			Spin::Controller::host_serial.print_string("\r");
		}
		if (Spin::Controller::host_serial.HasEOL())
		{
			Spin::Controller::host_serial.Get();
			Spin::Control::Output::active_pid_mode->Kp+=0.01;
		}
		
		//Write the output in every loop. 
		OCR0A = Spin::Control::Output::active_pid_mode->Max_Val -abs(Spin::Control::Output::active_pid_mode->cv_control_variable);
	}
}