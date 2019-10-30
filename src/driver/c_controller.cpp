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
#include "../closed loop/c_position.h"
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

void Spin::Driver::Controller::initialize()
{
	
	HardwareAbstractionLayer::Core::initialize();
	
	Spin::Driver::Controller::host_serial = c_Serial(0, 115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Driver::Controller::host_serial.print_string("proto type\r\n");//<-- Send hello message
	
	HardwareAbstractionLayer::Core::start_interrupts();//<--Start HW interrupts
	Spin::Configuration::initiailize();//<--init and set default config
	Spin::Configuration::load();//<--load user settings. if none exist defaults will load
	Spin::Input::initialize();//<--init the hardware inputs
	Spin::Output::initialize();//<--init hardware optputs
	
	
	/*
	Some motor drivers can act in an unpredictable way if certain things are not done. For example
	the power solutions driver will sometimes turn on the motor for no real reason if both direction
	lines are set to low, and the high voltage to the driver is turned on.
	In order to prevent things like that from happening I am adding an __armed__
	variable to the inputs. Until the __armed__ flag is set to a 1, this controller will not do
	anything. The last thing I want is for my driver board to power cycle and the spindle of my cnc
	mill come to life while I dont expect it to!
	To solve this condition with the power solutions driver, the 5v logic power line from the motor
	driver board should be connected to the arming input on the control. That way the driver will
	not get any commands from the controller until after it is powered up.
	*/

	
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Free);//<--set both direction pins high
	//Just set and wait until __armed__ is set to high. Even if hell freezes over, we are going to wait
	//while (!Spin::Input::Controls.__armed__)
	//{
	//	//check the input states over and over.
	//	HardwareAbstractionLayer::Inputs::synch_hardware_inputs();
	//	//when armed is set to true, we can break the loop and run as normal.
	//}

	Spin::Driver::Controller::sync_out_in_control();//<--synch controller states with input values
	Spin::Output::set_direction(Spin::Input::Controls.direction);//<--set direction input says to run.
	
	
}

//63800 starts motor
void Spin::Driver::Controller::run()
{
	user_pos = 200;
	
	while (1)
	{
		HardwareAbstractionLayer::Inputs::synch_hardware_inputs();//<--read input states from hardware
		Spin::Driver::Controller::sync_out_in_control();//<--synch input/output control states
		HardwareAbstractionLayer::Encoder::Calculator.get_rpm();//<--check rpm, recalculate if its time
		Spin::Driver::Controller::check_pid_cycle();//<--check if pid time has expired, and update if needed
		Spin::Driver::Controller::process();//<--General processing. Perhaps an LCD update
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
		Spin::ClosedLoop::Pid::Restart();
	}
	
	//Set the output mode to the mode specified by input
	//Velocity modes direction is controlled by the user. Positions direction
	//is controlled itnernally to reach its target
	if (Spin::Input::Controls.in_mode != Spin::Enums::e_drive_modes::Position
	&& (Spin::Input::Controls.direction != Spin::Output::Controls.direction))
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
				Spin::ClosedLoop::Position::step(Spin::Input::Controls.target, spindle_encoder.position);
				break;
			}
			default:
			/* Your code here */
			break;
		}
	}
}

void Spin::Driver::Controller::process()
{
	
	//if (BitTst(extern_input__intervals, RPT_INTERVAL_BIT)) //<--one second interval for general purpose reporting
	{
		BitClr_(extern_input__intervals, RPT_INTERVAL_BIT);

		Spin::Driver::Controller::host_serial.print_string(" (P):");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::active_factors->Kp);
		Spin::Driver::Controller::host_serial.print_string(" (I):");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::active_factors->Ki);
		Spin::Driver::Controller::host_serial.print_string(" (D):");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::active_factors->Kd);
		Spin::Driver::Controller::host_serial.print_string(" (A)ccel:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Configuration::User_Settings.Motor_Accel_Rate_Per_Second);
		Spin::Driver::Controller::host_serial.print_string(" (E)nc:");
		Spin::Driver::Controller::host_serial.print_int32((int)Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode);
		Spin::Driver::Controller::host_serial.print_string(" (C)ppr:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value);
		Spin::Driver::Controller::host_serial.print_string(" (U)SAVE");
		Spin::Driver::Controller::host_serial.print_string(" (R)RESET");

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
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::errors.process);
		Spin::Driver::Controller::host_serial.print_string(" dir:");
		Spin::Driver::Controller::host_serial.print_int32((int)Spin::Output::Controls.direction);
		Spin::Driver::Controller::host_serial.print_string(" p_pid:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::output);
		Spin::Driver::Controller::host_serial.print_string(" raw_pid:");
		Spin::Driver::Controller::host_serial.print_int32(Spin::ClosedLoop::Pid::raw_output);
		

		if (Spin::Driver::Controller::host_serial.HasEOL())
		{
			uint8_t byte = toupper(Spin::Driver::Controller::host_serial.Peek());
			if (byte == 'P' || byte == 'I' || byte == 'D' || byte == 'A' || byte == 'E' || byte == 'C' || byte == 'R')
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
				Spin::ClosedLoop::Pid::active_factors->Kp = p_var;
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else if (byte == 'I')
			{
				Spin::ClosedLoop::Pid::active_factors->Ki = p_var;
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else if (byte == 'D')
			{
				Spin::ClosedLoop::Pid::active_factors->Kd = p_var;
				Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Input::Controls.in_mode);
			}
			else if (byte == 'A')
			{
				Spin::Configuration::User_Settings.Motor_Accel_Rate_Per_Second = p_var;
				Spin::ClosedLoop::Velocity::Acceleration_Per_Cycle = pow((Spin::Configuration::User_Settings.Motor_Accel_Rate_Per_Second * (1 / PID_PERIODS_IN_INTERVAL)),2);
			}
			else if (byte == 'U')
			{
				Spin::Configuration::save();
				Spin::Driver::Controller::host_serial.print_string(" \r\nSAVED!!!");
			}
			else if (byte == 'E')
			{
				Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = (Spin::Enums::e_encoder_modes) p_var;
				Spin::Configuration::_assign_encoder_vectors(Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode);
			}
			else if (byte == 'C')
			{
				Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value = p_var;
			}
			else if (byte == 'R')
			{
				char stream[96];
				memcpy(stream,0,96);
				HardwareAbstractionLayer::Storage::save(stream, sizeof(stream));
				Spin::Driver::Controller::host_serial.print_string(" \r\nRESET!!!");
			}
			else
			{
				user_pos = p_var;
			}
			Spin::Driver::Controller::host_serial.SkipToEOL();


			Spin::Driver::Controller::host_serial.Reset();
		}

		Spin::Driver::Controller::host_serial.print_string("\r\n");

	}


}