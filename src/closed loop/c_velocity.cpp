/*
* c_pid.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

#include "c_velocity.h"
#include "../hardware_def.h"
#include "c_pid.h" //<--Also includes c_configuration.h
#include <math.h>

static int32_t pwm_out = 0;
static Spin::Enums::e_velocity_states State;
static uint16_t some_rmp_range = 5;
uint16_t Spin::ClosedLoop::Velocity::Acceleration_Per_Cycle = 0;

void Spin::ClosedLoop::Velocity::step(int32_t target, int32_t actual)
{
	//See if the target value is in range. If not ignore it all together.
	//Spin::ClosedLoop::Pid::scaler = 1;
	if (!_check_range(target))
	return;

	_set_state(target, actual);
	
	target = _clamp_acceleration(target, actual);

	//First order of business is to get the pid value
	Spin::ClosedLoop::Pid::Calculate(target, actual);

	//Do we want to hold EXACTLY the requested RPM?
	if (!_check_tolerance(abs(Spin::ClosedLoop::Pid::errors.process)))
	return;

	////Set the state of the velocity control
	//if (Spin::ClosedLoop::Pid::errors.direction > 0)
	//State = Spin::Enums::e_velocity_states::Accelerate;
	//else if (Spin::ClosedLoop::Pid::errors.direction < 0)
	//State = Spin::Enums::e_velocity_states::Decelerate;
	//else if (Spin::ClosedLoop::Pid::errors.direction == 0)
	//State = Spin::Enums::e_velocity_states::Cruise;

	//If we made it here we are going to use the PIDs calcualted output value.
	pwm_out = Spin::ClosedLoop::Pid::output;
	
	//Send the output value to the hardware
	HardwareAbstractionLayer::Outputs::update_output(pwm_out);

}

bool Spin::ClosedLoop::Velocity::_check_range(int32_t target_rpm)
{
	//see if we are being requested to go faster/slower than the rpm range settings
	if (Spin::Configuration::User_Settings.Motor_Max_RPM != -1
	&& target_rpm > Spin::Configuration::User_Settings.Motor_Max_RPM)
	return false;//<--ignore a value that is too high
	if (Spin::Configuration::User_Settings.Motor_Min_RPM != -1
	&& target_rpm < Spin::Configuration::User_Settings.Motor_Min_RPM)
	return false;//<--ignore a value that is too low

	
	return true;//<-- value is 'in range'
}

bool Spin::ClosedLoop::Velocity::_check_tolerance(int32_t target_rpm)
{
	//see if we are being requested to adjust a value that is still within tolerance
	if ((Spin::Configuration::User_Settings.Motor_RPM_Error != -1)
	&& (target_rpm < Spin::Configuration::User_Settings.Motor_RPM_Error))
	return false;//<--ignore a value that is within tolerance
	

	return true;//<-- value is out of tolerance
}

void Spin::ClosedLoop::Velocity::_set_state(int32_t target, int32_t actual)
{
	//Set states
	if ((target - Spin::Configuration::User_Settings.Motor_RPM_Error) > actual)
	{
		State = Spin::Enums::e_velocity_states::Accelerate;
	}
	else if ((target + Spin::Configuration::User_Settings.Motor_RPM_Error) < actual)
	{
		State = Spin::Enums::e_velocity_states::Decelerate;
	}
	else
	{
		//we are at speed, do nothing
		State = Spin::Enums::e_velocity_states::Cruise;
	}
}

int32_t Spin::ClosedLoop::Velocity::_clamp_acceleration(int32_t target, int32_t actual)
{
	//Set states
	if (State == Spin::Enums::e_velocity_states::Accelerate)
	{
		if ((target - actual)>Acceleration_Per_Cycle)
		{
			//clamp accel output
			target = actual + (Acceleration_Per_Cycle + 1);
		}
		//else
		//{
		//	//handing this over to pid entirely now
		//	State = Spin::Enums::e_velocity_states::Cruise;
		//}
	}
	else if (State == Spin::Enums::e_velocity_states::Decelerate)
	{
		if ((actual - target)>Acceleration_Per_Cycle)
		{
			//clamp decel output
			target = actual - (Acceleration_Per_Cycle + 1);
		}
		//else
		//{
		//	//handing this over to pid entirely now
		//	State = Spin::Enums::e_velocity_states::Cruise;
		//}
	}
	return target;
}
