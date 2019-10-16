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
#include "c_enumerations.h"
#include "bit_manipulation.h"
#include "volatile_encoder_externs.h"
#include "volatile_input_externs.h"


//#define DEBUG_AVR
//#define DEBUG_AVR_PID
//#define MOTOR_AVR_TUNER
#ifdef MSVC
#define DEBUG_WIN32
#endif

c_Serial Spin::Controller::host_serial;

//uint8_t Spin::Controller::pid_interval = 0;
//uint8_t Spin::Controller::one_interval = 0;
static uint16_t user_pos = 0;
static int32_t error_amount = 0;
void Spin::Controller::initialize()
{
	Spin::Controller::host_serial = c_Serial(0, 115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Controller::host_serial.print_string("proto type\r\n");//<-- Send hello message



	//enable interrupts
	HardwareAbstractionLayer::Core::start_interrupts();
	
	Spin::Configuration::initiailize();
	
	Spin::Input::initialize();
	
	Spin::Output::initialize();
	
	Spin::Controller::sync_out_in_control();
	
	//set the direction specified by the input pins
	Spin::Output::set_direction(Spin::Input::Controls.direction);

}
//63800 starts motor
void Spin::Controller::run()
{
	user_pos = 2000;
	
	while (1)
	{

		HardwareAbstractionLayer::Inputs::synch_hardware_inputs();//<--read input states from hardware
		Spin::Controller::sync_out_in_control();//<--synch input/output control states
		HardwareAbstractionLayer::Encoder::get_rpm();//<--check rpm, recalculate if its time
		//HardwareAbstractionLayer::Inputs::get_set_point();//<--get set point if its time
		//Spin::Input::Controls.sensed_position = extern_encoder__count;
		//Spin::Input::Controls.step_counter = extern_input__time_count;
		Spin::Controller::check_pid_cycle();//<--check if pid time has expired, and update if needed

		//if (Spin::Output::Controls.enable == Enums::e_drive_states::Enabled)
		{
			//HardwareAbstractionLayer::Outputs::update_output(62500);

			Spin::Controller::process();//<--General processing. Perhaps an LCD update
		}
	}
}

void Spin::Controller::sync_out_in_control()
{
	
	if (Spin::Input::Controls.enable != Spin::Output::Controls.enable)
	{
		/*if soft stops are configured (motor free spins to a stop)
		set both direction pins high and the motor can free wheel to
		a complete stop
		*/
		//Are we commanded to stop?
		//if (Spin::Input::Controls.enable == Spin::Enums::e_drive_states::Disabled
		//&& !Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable)
		//{
			//Spin::Input::Controls.direction = Enums::e_directions::Free;
		//}
	}
	
	//Set the output mode to the mode specified by input
	if (Spin::Input::Controls.direction != Spin::Output::Controls.direction)
	{
		
		
		//If mode changes we also need to reset the PID control.
		//set_mode will do that for us
		Spin::Output::set_direction(Spin::Input::Controls.direction);
	}

	//Set the output mode to the mode specified by input
	if (Spin::Input::Controls.in_mode != Spin::Output::Controls.out_mode)
	{
		//If mode changes we also need to reset the PID control.
		//set_mode will do that for us
		Spin::Output::set_mode(Spin::Input::Controls.in_mode);
	}
	
	//Synch outputs enable state to input state
	if (Spin::Input::Controls.enable != Spin::Output::Controls.enable)
	{
		
		//Start/stop pwm output pin, and stop/start pwm signal
		Spin::Output::set_drive_state(Spin::Input::Controls.enable);
	}
	
}

void Spin::Controller::check_pid_cycle()
{

	if (!BitTst(extern_input__intervals,PID_INTERVAL_BIT))
	return;//<--return if its not time
	
	BitClr_(extern_input__intervals,PID_INTERVAL_BIT);//<--clear this bit because we are running the pid loop
	
	if ( Spin::Input::Controls.enable == Enums::e_drive_states::Enabled) //<--is drive enabled
	{
		//calculate the change for the mode we are in
		switch (Spin::Output::Controls.out_mode)
		{
			case Enums::e_drive_modes::Velocity:
			{
				//update input value
				Spin::Input::Controls.target = user_pos;
				//Spin::Input::Controls.target = extern_input__time_count;
				//set the direction specified by the input pins
				//Spin::Output::set_direction(Spin::Input::Controls.direction);
				Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.target, spindle_encoder.sensed_rpm);
				break;
			}
			case Enums::e_drive_modes::Position:
			{
				//figure out which direction is closer!

				Spin::Input::Controls.target = user_pos;
				//Spin::Input::Controls.target = extern_input__time_count;
				error_amount = ((int32_t)Spin::Input::Controls.target - spindle_encoder.position);
				if (((int32_t)Spin::Input::Controls.target - spindle_encoder.position) < 0)
				{
					//changing direction will be a shorter path to the target
					Spin::Output::active_pid_mode->control_direction = Enums::e_directions::Reverse;
					//if we have changed directions reset the integral, or we have to wait for it to unwind
					//TODO: Cannot change direction after motion starts. Mst set this before we start moving.
					Spin::Output::active_pid_mode->reset_integral();
				}
				else if (((int32_t)Spin::Input::Controls.target - spindle_encoder.position) > 0)
				{
					//changing direction will be a shorter path to the target
					Spin::Output::active_pid_mode->control_direction = Enums::e_directions::Forward;
					//if we have changed directions reset the integral, or we have to wait for it to unwind
					//TODO: Cannot change direction after motion starts. Mst set this before we start moving.
					Spin::Output::active_pid_mode->reset_integral();
				}

				//this is too touchy for PWM with interference. Hard setting a position value for testing
				Spin::Output::active_pid_mode->get_pid(Spin::Input::Controls.target, spindle_encoder.position);

				//in position mode pid can return a + or - value.
				//the direction flag should already be set.
				Spin::Output::set_direction(Spin::Output::active_pid_mode->control_direction);
				
				break;
			}
			default:
			/* Your code here */
			break;
		}
		if (Spin::Output::active_pid_mode != NULL)
		HardwareAbstractionLayer::Outputs::update_output(Spin::Output::active_pid_mode->pid_calc.output);
		else
		Spin::Controller::host_serial.print_string(" PID select error\r\n");
	}
	else
	{
		#ifdef DEBUG_AVR
		Spin::Controller::host_serial.print_string(" pid not active\r\n");
		#endif
	}

}

void Spin::Controller::process()
{


	//if (BitTst(extern_input__intervals, RPT_INTERVAL_BIT)) //<--one second interval for general purpose reporting
	{
		BitClr_(extern_input__intervals, RPT_INTERVAL_BIT);

		Spin::Controller::host_serial.print_string(" mod:");
		Spin::Controller::host_serial.print_int32((int)Spin::Input::Controls.in_mode);
		Spin::Controller::host_serial.print_string(" ena:");
		Spin::Controller::host_serial.print_int32((int)Spin::Input::Controls.enable);
		Spin::Controller::host_serial.print_string(" rpm:");
		Spin::Controller::host_serial.print_int32(spindle_encoder.sensed_rpm);
		Spin::Controller::host_serial.print_string(" pos:");
		Spin::Controller::host_serial.print_int32(spindle_encoder.position);
		Spin::Controller::host_serial.print_string(" trg:");
		Spin::Controller::host_serial.print_int32(Spin::Input::Controls.target);
		Spin::Controller::host_serial.print_string(" err:");
		Spin::Controller::host_serial.print_int32(error_amount);
		Spin::Controller::host_serial.print_string(" dir:");
		Spin::Controller::host_serial.print_int32((int)Spin::Input::Controls.direction);
		
		if (Spin::Output::active_pid_mode !=NULL)
		{
			Spin::Controller::host_serial.print_string(" p_pid:");
			Spin::Controller::host_serial.print_int32(Spin::Output::active_pid_mode->pid_calc.output);
			
		}

		if (Spin::Controller::host_serial.HasEOL())
		{
			uint8_t byte = toupper(Spin::Controller::host_serial.Peek());
			if (byte == 'P' || byte == 'I' || byte == 'D')
			{
				Spin::Controller::host_serial.Get();

			}

			char  num[10]{0, 0, 0, 0,0, 0, 0, 0, 0, 0};
			char * _num;
			_num = num;
			for (int i = 0; i < 10; i++)
			{
				uint8_t byte = toupper(Spin::Controller::host_serial.Peek());
				if (byte == 13)
					break;
				num[i] = Spin::Controller::host_serial.Get();
			}
			uint16_t p_var = 0;
			p_var = atoi(_num);


			if (byte == 'P')
			{
				Spin::Configuration::PID_Tuning.Position.Kp = p_var;
				Spin::Controller::host_serial.print_string(" pid P:");
				Spin::Controller::host_serial.print_int32(p_var);
				Spin::Output::set_pid_values();
			}
			else if (byte == 'I')
			{
				Spin::Configuration::PID_Tuning.Position.Ki = p_var;
				Spin::Controller::host_serial.print_string(" pid I:");
				Spin::Controller::host_serial.print_int32(p_var);
				Spin::Output::set_pid_values();
			}
			else if (byte == 'D')
			{
				Spin::Configuration::PID_Tuning.Position.Kd = p_var;
				Spin::Controller::host_serial.print_string(" pid D:");
				Spin::Controller::host_serial.print_int32(p_var);
				Spin::Output::set_pid_values();
			}
			else
			{
				user_pos = p_var;
				Spin::Controller::host_serial.print_string(" usr:");
				Spin::Controller::host_serial.print_int32(user_pos);
			}
			Spin::Controller::host_serial.SkipToEOL();


			Spin::Controller::host_serial.Reset();
		}

		Spin::Controller::host_serial.print_string("\r\n");

	}


}