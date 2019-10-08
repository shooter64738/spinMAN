/* 
* c_configuration.cpp
*
* Created: 10/6/2019 7:01:54 PM
* Author: Family
*/


#include "c_configuration.h"

Spin::Configuration::s_pid_tunings Spin::Configuration::PID_Tuning;
Spin::Configuration::s_drive_settings Spin::Configuration::Drive_Settings;

void Spin::Configuration::initiailize()
{
	Spin::Configuration::PID_Tuning.Position.Kp = 1;
	Spin::Configuration::PID_Tuning.Position.Ki = 0;
	Spin::Configuration::PID_Tuning.Position.Kd = 0;
	Spin::Configuration::PID_Tuning.Position.Allowed_Error_Percent = 100;

	Spin::Configuration::PID_Tuning.Velocity.Kp = 1;
	Spin::Configuration::PID_Tuning.Velocity.Ki = 0;
	Spin::Configuration::PID_Tuning.Velocity.Kd = 1;
	Spin::Configuration::PID_Tuning.Velocity.Allowed_Error_Percent = 100;

	Spin::Configuration::PID_Tuning.Torque.Kp = 1;
	Spin::Configuration::PID_Tuning.Torque.Ki = 0;
	Spin::Configuration::PID_Tuning.Torque.Kd = 0;
	Spin::Configuration::PID_Tuning.Torque.Allowed_Error_Percent = 50;

	Spin::Configuration::Drive_Settings.Drive_Output_Inverted = 1;
	Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable = 1;
}

void Spin::Configuration::load()
{
}

void Spin::Configuration::save()
{
}

void Spin::Configuration::modify()
{
}