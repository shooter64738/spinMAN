/*
* c_velocity.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

#include "c_position.h"
#include "../hardware_def.h"
#include "c_pid.h" //<--Also includes c_configuration.h

static Spin::Enums::e_position_states State;

void Spin::ClosedLoop::Position::step(int32_t target, int32_t actual)
{
	if (!_check_range(target))
	return;

	_set_state(target, actual);
	target = _clamp_acceleration(target, actual);

	//First order of business is to get the pid value
	Spin::ClosedLoop::Pid::Calculate(target, actual);

	//Do we want to hold EXACTLY the requested RPM?
	if (!_check_tolerance(abs(Spin::ClosedLoop::Pid::errors.process)))
	return;
}

bool Spin::ClosedLoop::Position::_check_range(int32_t target_position)
{
	return true;

	//see if we are being requested to go faster/slower than the rpm range settings
	if (Spin::Configuration::User_Settings.Motor_Max_RPM != -1
	&& target_position > Spin::Configuration::User_Settings.Motor_Max_RPM)
	return false;//<--ignore a value that is too high
	if (Spin::Configuration::User_Settings.Motor_Min_RPM != -1
	&& target_position < Spin::Configuration::User_Settings.Motor_Min_RPM)
	return false;//<--ignore a value that is too low

	
	return true;//<-- value is 'in range'
}

bool Spin::ClosedLoop::Position::_check_tolerance(int32_t target_position)
{
	return true;

	//see if we are being requested to adjust a value that is still within tolerance
	if ((Spin::Configuration::User_Settings.Motor_RPM_Error != -1)
	&& (target_position < Spin::Configuration::User_Settings.Motor_RPM_Error))
	return false;//<--ignore a value that is within tolerance
	

	return true;//<-- value is out of tolerance
}

void Spin::ClosedLoop::Position::_set_state(int32_t target, int32_t actual)
{
	//Set states
	if ((target - Spin::Configuration::User_Settings.Motor_RPM_Error) > actual)
	{
		State = Spin::Enums::e_position_states::Accel_To_Target;
	}
	else if ((target + Spin::Configuration::User_Settings.Motor_RPM_Error) < actual)
	{
		State = Spin::Enums::e_position_states::Target_Exceeded;
	}
	else
	{
		//we are at speed, do nothing
		State = Spin::Enums::e_position_states::On_Target;
	}
}

int32_t Spin::ClosedLoop::Position::_clamp_acceleration(int32_t target, int32_t actual)
{
	return target;
}
