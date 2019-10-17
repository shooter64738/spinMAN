/*
* c_configuration.cpp
*
* Created: 10/6/2019 7:01:54 PM
* Author: Family
*/


#include "c_configuration.h"
#include "../hardware_def.h"
#include "../bit_manipulation.h"
#include "../driver/c_input.h"
#include "../driver/c_controller.h"
#include "../driver/volatile_encoder_externs.h"
#include "../driver/volatile_input_externs.h"



Spin::Configuration::s_pid_tunings Spin::Configuration::PID_Tuning;
Spin::Configuration::s_drive_settings Spin::Configuration::Drive_Settings;
Spin::Configuration::s_user_settings Spin::Configuration::User_Settings;
Spin::Enums::e_config_results Spin::Configuration::Status;

void Spin::Configuration::initiailize()
{
	//Default max output to 255;
	Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT8_MAX;
	/*
	Try to get the maximum output the timer can have.
	*/
	switch (PWM_RESOLUTION_BIT)
	{
		case 32:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT32_MAX;
			break;
		}
		case 16:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT16_MAX;
			break;
		}
		case 8:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT8_MAX;
			break;
		}
	}
	HardwareAbstractionLayer::Encoder::initialize();
	Spin::Configuration::load_defaults();
}

void Spin::Configuration::load_defaults()
{
	//Some default pid values
	Spin::Configuration::PID_Tuning.Position.Kp = 1;
	Spin::Configuration::PID_Tuning.Position.Ki = 1;
	Spin::Configuration::PID_Tuning.Position.Kd = 1;
	Spin::Configuration::PID_Tuning.Position.Allowed_Error_Percent = 100;

	Spin::Configuration::PID_Tuning.Velocity.Kp = 25;
	Spin::Configuration::PID_Tuning.Velocity.Ki = 0;
	Spin::Configuration::PID_Tuning.Velocity.Kd = 0;
	Spin::Configuration::PID_Tuning.Velocity.Allowed_Error_Percent = 100;

	Spin::Configuration::PID_Tuning.Torque.Kp = 0;
	Spin::Configuration::PID_Tuning.Torque.Ki = 0;
	Spin::Configuration::PID_Tuning.Torque.Kd = 0;
	Spin::Configuration::PID_Tuning.Torque.Allowed_Error_Percent = 50;

	Spin::Configuration::Drive_Settings.Drive_Output_Inverted = 1;
	Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable = 1;

	//Default to either 0 or highest pwm value, depending on inversion
	Spin::Configuration::Drive_Settings.Drive_Turn_Off_Value
	= Drive_Settings.Drive_Output_Inverted ? Spin::Configuration::Drive_Settings.Max_PWM_Output : 0;

	//Default to either 0 or highest pwm value, depending on inversion. This may change when user config is loaded
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value
	= Drive_Settings.Drive_Output_Inverted ? Spin::Configuration::Drive_Settings.Max_PWM_Output : 0;

	Spin::Configuration::User_Settings.Motor_Max_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Motor_Min_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Home_Position = 0; //default to 0
	Spin::Configuration::User_Settings.Tool_Orientation = 0; //default to 0
	Spin::Configuration::User_Settings.Default_Direction = Enums::e_directions::Free;//set to free spin.
	
	
	
	//assume there has not been a config done.
	Spin::Configuration::Status = Spin::Enums::e_config_results::Incomplete_Config;
	
}

Spin::Enums::e_config_results Spin::Configuration::load()
{
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value = 64088;
	//Encoder has 100 pulses in a rotation.
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev = 400;
	//If quadrature mode is active the 100 pulses per rotation is multiplied by 4 (quadrature count)
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = Enums::e_encoder_modes::Quadrature;
	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_quad;
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_quad;
	
	return Enums::e_config_results::Incomplete_Config;
}

void Spin::Configuration::save()
{
}

void Spin::Configuration::auto_config(char * config_type)
{
	Spin::Configuration::_config_encoder();
}

Spin::Enums::e_config_results Spin::Configuration::_config_encoder()
{
	write_message("ENC_CFG:***Stand clear of motor!*** Press Enter to Start\r\n\0");
	//disable the drive
	HardwareAbstractionLayer::Outputs::disable_output();
	//configure the encoder for all signals
	HardwareAbstractionLayer::Encoder::config_cha();
	HardwareAbstractionLayer::Encoder::config_chb();
	HardwareAbstractionLayer::Encoder::config_chz();
	HardwareAbstractionLayer::Encoder::initialize();
	
	//Set output to half. 0 or max val might make the motor spin waaay to fast all at once.
	uint32_t current_output = Spin::Configuration::Drive_Settings.Max_PWM_Output/2;
	
	//set output to current
	HardwareAbstractionLayer::Outputs::enable_output();
	HardwareAbstractionLayer::Outputs::update_output(current_output);
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
	//Wait for 1000ms and see if we had encoder events
	//This interval bit is set by a timer, so we just
	//set and wait for it.
	
	//Spin::Driver::Controller::host_serial.print_string("chans:");
	//Spin::Driver::Controller::host_serial.print_int32(extern_encoder__active_channels);
	spindle_encoder.active_channels = 0;
	for (int i=0;i<6;i++)
	//while(1)
	{
		while (!BitTst(extern_input__intervals, ONE_INTERVAL_BIT))
		{
			/*
			since quadrature with index is all the features we can support, if we
			detect that encoder type, we can stop this loop early.
			*/
			HardwareAbstractionLayer::Encoder::get_active_channels();
			
			if (spindle_encoder.active_channels == (int)Enums::e_encoder_modes::Quadrature_wIndex)
			break;
		}
		extern_input__intervals &= ~(1<<ONE_INTERVAL_BIT);
		Spin::Driver::Controller::host_serial.print_string("=");
		//extern_encoder__active_channels = 0;
	}
	Spin::Driver::Controller::host_serial.print_string(">\r");
	if (spindle_encoder.active_channels == 0 )
	{
		write_message("ENC_NON\r\n\0");
		return Spin::Enums::e_config_results::Encoder_Not_Available ;
	}
	//stop the drive
	HardwareAbstractionLayer::Outputs::disable_output();
	HardwareAbstractionLayer::Outputs::update_output(0);
	
	
	/*
	active channels is a bit flag value
	binar value					decimal
	00000000 = no encoder		0
	00000001 = active chan a	1
	00000010 = active chan b	2
	00000011 = active chan ab	3
	00000100 = active chan z	4
	00000101 = active chan za	5
	00000110 = active chan zb	6
	00000111 = active chan zba	7
	
	therefore in decimal value it will match the enum values
	*/
	Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = (Spin::Enums::e_encoder_modes) spindle_encoder.active_channels;
	//Now that the encoder type is known we can set the pointer for the isrs to call when something happens
	

	Spin::Driver::Controller::host_serial.print_string("ENC_CHN:");
	//This will cover most encoders, but I handle quadrature and quadrature with index different
	if (spindle_encoder.active_channels & ENC_CHA_TRK_BIT)
	{
		spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_cha;
		Spin::Driver::Controller::host_serial.print_string("A");
	}
	if (spindle_encoder.active_channels & ENC_CHB_TRK_BIT)
	{
		spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_chb;
		Spin::Driver::Controller::host_serial.print_string("B");
	}
	if (spindle_encoder.active_channels & ENC_CHZ_TRK_BIT)
	{
		spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::read_chz;
		Spin::Driver::Controller::host_serial.print_string("Z");
	}
	//Special handler for quadrature because they report direction too.
	if (Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature
	|| Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature_wIndex)
	{
		Spin::Driver::Controller::host_serial.print_string(" QD");
		spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_quad;
		spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_quad;
		if (Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature_wIndex)
		{
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_chz;
			Spin::Driver::Controller::host_serial.print_string(" I");
		}
	}
	Spin::Driver::Controller::host_serial.print_string("\r\n");
	//now ask the user for the PPR value.
	Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev = 0;
	while (Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev == 0)
	{
		Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev = input_int32("ENC_PPR?\r\n\0");
		if (Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev < 0)
		{
			write_message("ENC_FAL\r\n\0");
			Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev = 0;
			return Spin::Enums::e_config_results::Encoder_Not_Available;
		}
		break;
	}
	
	//Here we need to run a forward/reverse test to determine the proper direction of the encoder
	
	//Clear the encoder position
	spindle_encoder.position = 0;
	Spin::Driver::Controller::host_serial.print_string("ENC_DIR\r");
	//extern_encoder__direction = 0;
	
	//set output to current
	HardwareAbstractionLayer::Outputs::enable_output();
	HardwareAbstractionLayer::Outputs::update_output(current_output);
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
	
	for (int i=0;i<6;i++)
	//while(1)
	{
		while (!BitTst(extern_input__intervals, ONE_INTERVAL_BIT))
		{
			/*
			since quadrature with index is all the features we can support, if we
			detect that encoder type, we can stop this loop early.
			*/
			HardwareAbstractionLayer::Encoder::get_active_channels();
			
			if (spindle_encoder.direction != 0)
			break;
		}
		extern_input__intervals &= ~(1<<ONE_INTERVAL_BIT);
		Spin::Driver::Controller::host_serial.print_string("=");
		//extern_encoder__active_channels = 0;
	}
	HardwareAbstractionLayer::Outputs::disable_output();
	Spin::Driver::Controller::host_serial.print_string(">\r");
	Spin::Driver::Controller::host_serial.print_string("ENC_DIR:");
	Spin::Driver::Controller::host_serial.print_int32(spindle_encoder.direction);
	Spin::Driver::Controller::host_serial.print_string("DIR_FWD\r");
	write_message("ENC_OK\r\n\0");
	
	
	
	write_message("ENC_OK\r\n\0");
	return Spin::Enums::e_config_results::Encoder_Available;
}

void Spin::Configuration::write_message(char * message)
{
	Spin::Driver::Controller::host_serial.print_string(message);
	while(!Spin::Driver::Controller::host_serial.HasEOL())
	{
	}
	Spin::Driver::Controller::host_serial.SkipToEOL();
	Spin::Driver::Controller::host_serial.Reset();
}
char* Spin::Configuration::input_char()
{
	return NULL;
}

int32_t Spin::Configuration::input_int32(char * message)
{
	write_message(message);
	while(!Spin::Driver::Controller::host_serial.HasEOL())
	{
		if (Spin::Driver::Controller::host_serial.HasEOL())
		{
			char  num[INPUT_NUM_SIZE];
			char * _num;
			_num = num;
			memcpy(num,0,sizeof(int32_t));
			num[0] = Spin::Driver::Controller::host_serial.Get();
			
			for (int i = 1; i < INPUT_NUM_SIZE; i++)
			{
				if (Spin::Driver::Controller::host_serial.Peek() == 13
				|| Spin::Driver::Controller::host_serial.Peek() == 10)
				{
					Spin::Driver::Controller::host_serial.SkipToEOL();
					Spin::Driver::Controller::host_serial.Reset();
					break; // break the for loop
				}
				if (Spin::Driver::Controller::host_serial.Peek() == '\\') //user canceled input
				return -1;
				num[i] = Spin::Driver::Controller::host_serial.Get();
			}
			int32_t _var = 0;
			_var = atoi(_num);
			return _var;
		}
	}
}
uint32_t Spin::Configuration::input_uint32(char * message)
{
	return 0;
}