/*
* c_controller.cpp
*
* Created: 10/3/2019 4:29:26 PM
* Author: jeff_d
*/


#include "c_controller.h"
#include "c_input.h"
#include "c_output.h"
#include "../core/c_configuration.h"
#include "../core/c_enumerations.h"
#include "../bit_manipulation.h"
#include "volatile_encoder_externs.h"
#include "volatile_input_externs.h"
#include "../closed loop/c_velocity.h"
#include "../closed loop/c_pid.h"


//#define DEBUG_AVR
//#define DEBUG_AVR_PID
//#define MOTOR_AVR_TUNER
#ifdef MSVC
#define DEBUG_WIN32
#endif

c_Serial Spin::Driver::Controller::host_serial;

//uint8_t Spin::Driver::Controller::pid_interval = 0;
//uint8_t Spin::Driver::Controller::one_interval = 0;
static uint16_t user_pos = 0;
static int32_t error_amount = 0;

void Spin::Driver::Controller::initialize()
{
	Spin::Driver::Controller::host_serial = c_Serial(0, 115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Driver::Controller::host_serial.print_string("proto type\r\n");//<-- Send hello message



	//enable interrupts
	HardwareAbstractionLayer::Core::start_interrupts();
	
	Spin::Configuration::initiailize();
	Spin::Configuration::load();
	
	Spin::Input::initialize();
	
	Spin::Output::initialize();
	
	Spin::Driver::Controller::sync_out_in_control();
	
	//set the direction specified by the input pins
	Spin::Output::set_direction(Spin::Input::Controls.direction);
	Spin::ClosedLoop::Pid::Set_Factors(Configuration::PID_Tuning.Velocity);

}
//63800 starts motor
void Spin::Driver::Controller::run()
{
	user_pos = 2000;
	
	while (1)
	{

		HardwareAbstractionLayer::Inputs::synch_hardware_inputs();//<--read input states from hardware
		Spin::Driver::Controller::sync_out_in_control();//<--synch input/output control states
		HardwareAbstractionLayer::Encoder::get_rpm();//<--check rpm, recalculate if its time
		//HardwareAbstractionLayer::Inputs::get_set_point();//<--get set point if its time
		//Spin::Input::Controls.sensed_position = extern_encoder__count;
		//Spin::Input::Controls.step_counter = extern_input__time_count;
		Spin::Driver::Controller::check_pid_cycle();//<--check if pid time has expired, and update if needed

		//if (Spin::Output::Controls.enable == Enums::e_drive_states::Enabled)
		{
			//HardwareAbstractionLayer::Outputs::update_output(62500);

			Spin::Driver::Controller::process();//<--General processing. Perhaps an LCD update
		}
	}
}

void Spin::Driver::Controller::sync_out_in_control()
{
	
	if (Spin::Input::Controls.enable != Spin::Output::Controls.enable)
	{
		/*if soft stops are configured (motor free spins to a stop)
		set both direction pins high and the motor can free wheel to
		a complete stop
		*/
		//Are we commanded to stop?
		if (Spin::Input::Controls.enable == Spin::Enums::e_drive_states::Disabled
		&& !Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable)
		{
			
			//Spin::Input::Controls.direction = Enums::e_directions::Free;
		}
		Spin::Driver::Controller::host_serial.print_string("RESTARTING!!!\r\n");
		Spin::ClosedLoop::Pid::Restart();
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

void Spin::Driver::Controller::check_pid_cycle()
{

	if (!BitTst(extern_input__intervals,PID_INTERVAL_BIT))
	return;//<--return if its not time
	
	BitClr_(extern_input__intervals,PID_INTERVAL_BIT);//<--clear this bit because we are running the pid loop
	
	if ( Spin::Input::Controls.enable == Enums::e_drive_states::Enabled) //<--is drive enabled
	{
		/*
		When mode was set the pid computer should have been assigned the correct tuning values.
		We assume that is the case when we get in this switch statement
		*/
		//calculate the change for the mode we are in
		switch (Spin::Output::Controls.out_mode)
		{
			case Enums::e_drive_modes::Velocity:
			{
				/*
				It turned out to be more complex tan just updating a PID value for these different
				modes, so I have moved each mode to its own class to do those mode specific operations
				*/
				//update input value
				Spin::Input::Controls.target = user_pos;
				Spin::ClosedLoop::Velocity::step(Spin::Input::Controls.target, spindle_encoder.sensed_rpm);
				break;
			}
			case Enums::e_drive_modes::Position:
			{
				Spin::Input::Controls.target = user_pos;
				Spin::ClosedLoop::Pid::Calculate(Spin::Input::Controls.target, spindle_encoder.position);
				break;
			}
			default:
			/* Your code here */
			break;
		}
	}
	else
	{
		#ifdef DEBUG_AVR
		Spin::Driver::Controller::host_serial.print_string(" pid not active\r\n");
		#endif
	}

}

void Spin::Driver::Controller::process()
{


	if (BitTst(extern_input__intervals, RPT_INTERVAL_BIT)) //<--one second interval for general purpose reporting
	{
		BitClr_(extern_input__intervals, RPT_INTERVAL_BIT);

		Spin::Driver::Controller::host_serial.print_string(" mod:");
		Spin::Driver::Controller::host_serial.print_int32((int)Spin::Input::Controls.in_mode);
		Spin::Driver::Controller::host_serial.print_string(" ena:");
		Spin::Driver::Controller::host_serial.print_int32((int)Spin::Input::Controls.enable);
		Spin::Driver::Controller::host_serial.print_string(" rpm:");
		Spin::Driver::Controller::host_serial.print_int32(spindle_encoder.sensed_rpm);
		Spin::Driver::Controller::host_serial.print_string(" pos:");
		Spin::Driver::Controller::host_serial.print_int32(spindle_encoder.position);
		Spin::Driver::Controller::host_serial.print_string(" trg:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Input::Controls.target);
		Spin::Driver::Controller::host_serial.print_string(" err:");
		Spin::Driver::Controller::host_serial.print_int32(error_amount);
		Spin::Driver::Controller::host_serial.print_string(" dir:");
		Spin::Driver::Controller::host_serial.print_int32((int)Spin::Input::Controls.direction);
		Spin::Driver::Controller::host_serial.print_string(" p_pid:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::output);
		Spin::Driver::Controller::host_serial.print_string(" p_trm:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Configuration::PID_Tuning.Velocity.Kp);
		Spin::Driver::Controller::host_serial.print_string(" i_trm:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Configuration::PID_Tuning.Velocity.Ki);
		Spin::Driver::Controller::host_serial.print_string(" d_trm:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Configuration::PID_Tuning.Velocity.Kd);

		if (Spin::Driver::Controller::host_serial.HasEOL())
		{
			uint8_t byte = toupper(Spin::Driver::Controller::host_serial.Peek());
			if (byte == 'P' || byte == 'I' || byte == 'D')
			{
				Spin::Driver::Controller::host_serial.Get();

			}

			char  num[10]{0, 0, 0, 0,0, 0, 0, 0, 0, 0};
			char * _num;
			_num = num;
			for (int i = 0; i < 10; i++)
			{
				uint8_t test = toupper(Spin::Driver::Controller::host_serial.Peek());
				if (test == 13)
				break;
				num[i] = Spin::Driver::Controller::host_serial.Get();
			}
			uint16_t p_var = 0;
			p_var = atoi(_num);


			if (byte == 'P')
			{
				Spin::Configuration::PID_Tuning.Velocity.Kp = p_var;
				Spin::Driver::Controller::host_serial.print_string(" pid P:");
				Spin::Driver::Controller::host_serial.print_int32(p_var);
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else if (byte == 'I')
			{
				Spin::Configuration::PID_Tuning.Velocity.Ki = p_var;
				Spin::Driver::Controller::host_serial.print_string(" pid I:");
				Spin::Driver::Controller::host_serial.print_int32(p_var);
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else if (byte == 'D')
			{
				Spin::Configuration::PID_Tuning.Velocity.Kd = p_var;
				Spin::Driver::Controller::host_serial.print_string(" pid D:");
				Spin::Driver::Controller::host_serial.print_int32(p_var);
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else
			{
				user_pos = p_var;
				Spin::Driver::Controller::host_serial.print_string(" usr:");
				Spin::Driver::Controller::host_serial.print_int32(user_pos);
			}
			Spin::Driver::Controller::host_serial.SkipToEOL();


			Spin::Driver::Controller::host_serial.Reset();
		}

		Spin::Driver::Controller::host_serial.print_string("\r\n");

	}


}