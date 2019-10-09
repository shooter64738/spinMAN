/*
* c_configuration.cpp
*
* Created: 10/6/2019 7:01:54 PM
* Author: Family
*/


#include "c_configuration.h"
#include "hardware_def.h"
#include "bit_manipulation.h"
#include "c_input.h"


Spin::Configuration::s_pid_tunings Spin::Configuration::PID_Tuning;
Spin::Configuration::s_drive_settings Spin::Configuration::Drive_Settings;
Spin::Configuration::s_user_settings Spin::Configuration::User_Settings;

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

	Spin::Configuration::load_defaults();

}

void Spin::Configuration::load_defaults()
{
	//Some default pid values
	Spin::Configuration::PID_Tuning.Position.Kp = 1;
	Spin::Configuration::PID_Tuning.Position.Ki = 1;
	Spin::Configuration::PID_Tuning.Position.Kd = 1;
	Spin::Configuration::PID_Tuning.Position.Allowed_Error_Percent = 100;

	Spin::Configuration::PID_Tuning.Velocity.Kp = 1;
	Spin::Configuration::PID_Tuning.Velocity.Ki = 1;
	Spin::Configuration::PID_Tuning.Velocity.Kd = 1;
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
	Spin::Configuration::Drive_Settings.Drive_Minimum_On_Value
	= Drive_Settings.Drive_Output_Inverted ? Spin::Configuration::Drive_Settings.Max_PWM_Output : 0;

	Spin::Configuration::User_Settings.Motor_Max_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Motor_Min_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Home_Position = 0; //default to 0
	Spin::Configuration::User_Settings.Tool_Orientation = 0; //default to 0
	Spin::Configuration::User_Settings.Default_Direction = Enums::e_directions::Free;//set to free spin.
	
	//Encoder has 100 pulses in a rotation.
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev = 100;
	//If quadrature mode is active the 100 pulses per rotation is multiplied by 4 (quadrature count)
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = Enums::e_encoder_modes::Quadrature;
	Platform_Specific_HAL_Encoder_Vector = HardwareAbstractionLayer::Encoder::update_encoder_for_quad;
	
}

void Spin::Configuration::load()
{
}

void Spin::Configuration::save()
{
}

void Spin::Configuration::auto_config(char * config_type)
{
	Spin::Configuration::_config_encoder();
}

void Spin::Configuration::_config_encoder()
{
	//disable the drive
	HardwareAbstractionLayer::Outputs::disable_output();
	//configure the encoder for quadrature mode
	HardwareAbstractionLayer::Inputs::configure_encoder_quadrature();
	
	//Set output to half. 0 or max val might make the motor spin waaay to fast all at once. 
	uint32_t current_output = MAX_PWM_VALUE/2;
	uint8_t active_channels = 0;
	//while(1)
	{
		//set output to current
		HardwareAbstractionLayer::Outputs::update_output(current_output);
		//Wait for 1000ms and see if we had encoder events
		//This interval bit is set by a timer, so we just
		//set and wait for it.
		while(!BitTst(HardwareAbstractionLayer::Inputs::get_intervals(),ONE_INTERVAL_BIT))
		{
			active_channels = HardwareAbstractionLayer::Inputs::get_encoder_active();
			/*
			since quadrature with index is all the features we can support, if we
			detect that encoder type, we can stop this loop early.
			*/
			if (active_channels == (int)Enums::e_encoder_modes::Quadrature_wIndex)
			break;
		}
		//see if we had input on either channel
		//if there was input on either channel we are done here.
		if (active_channels > 0)
		{
			//break;
		}
		
	}
	//set output to current
	HardwareAbstractionLayer::Outputs::disable_output();
	HardwareAbstractionLayer::Outputs::update_output(0);
	
	
	/*
	active channels is a bit flag value
	00000001 = active on chan a only
	00000011 = active chan a and b
	00000010 = active chan b only
	therefore in decimal value
	1 = active on channel a only
	2 = active on channel b only
	3 = active on both channels
	The encoder type enum matches this so theres nothing to convert or change
	just set the value.
	*/
	Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = (Spin::Enums::e_encoder_modes) active_channels;
	//Now that the encoder type is known we can set the pointer for the isrs to call when something happens
	switch (Configuration::Drive_Settings.Encoder_Config.Encoder_Mode)
	{
		case Spin::Enums::e_encoder_modes::No_Encoder :
		{
		/* Your code here */
		break;
		}
		case Spin::Enums::e_encoder_modes::Index_Only :
		{
			/* Your code here */
			break;
		}
		case Spin::Enums::e_encoder_modes::Quadrature :
		{
			Spin::Configuration::Drive_Settings.Encoder_Config.call_vect = HardwareAbstractionLayer::Inputs::update_encoder_for_quad;
			break;
		}
		case Spin::Enums::e_encoder_modes::Quadrature_wIndex :
		{
			/* Your code here */
			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanA :
		{
			/* Your code here */
			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanA_wIndex :
		{
			/* Your code here */
			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanB :
		{
			/* Your code here */
			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanB_wIndex :
		{
			/* Your code here */
			break;
		}
		
	}

}