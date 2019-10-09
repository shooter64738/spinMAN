/*
* c_configuration.cpp
*
* Created: 10/6/2019 7:01:54 PM
* Author: Family
*/


#include "c_configuration.h"
#include "hardware_def.h"

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
	Spin::Configuration::User_Settings.Default_Direction = Controller::e_directions::Free;//set to free spin.
}

void Spin::Configuration::load()
{
}

void Spin::Configuration::save()
{
}