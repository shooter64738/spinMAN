/*
* c_velocity.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

#include "c_position.h"
#include "../hardware_def.h"
#include "c_pid.h" //<--Also includes c_configuration.h
#include "../driver/volatile_encoder_externs.h"
#include "../driver/c_output.h"
#include "../driver/c_controller.h"


static int32_t pwm_out = 0;
static Spin::Enums::e_position_states State;


void Spin::ClosedLoop::Position::step(int32_t target, int32_t actual)
{
	//if (!_check_range(target))
	//return;
	Spin::ClosedLoop::Pid::scaler = 1.0;
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value = 64500;
	Spin::Configuration::User_Settings.Motor_Position_Error = 10;

	_set_state(target, actual);
	target = _clamp_acceleration(target, actual);
	int32_t nearest_direction = (target - actual) - (spindle_encoder.ticks_per_rev / 2);

	State = nearest_direction < 0 ? Spin::Enums::e_position_states::Accel_To_Target_Forward
		: Spin::Enums::e_position_states::Accel_To_Target_Reverse;

	if (nearest_direction < 0)
	{
		State = Spin::Enums::e_position_states::Accel_To_Target_Forward;
		if (Spin::Output::Controls.direction != Spin::Enums::e_directions::Forward)
		{
			//but we currently arent going forward, change to forward direction
			Spin::Output::set_direction(Spin::Enums::e_directions::Forward);
			//since we toggled directions, we should reset the integral windup
			Spin::ClosedLoop::Pid::Reset_integral();
		}
	}
	else
	{
		State = Spin::Enums::e_position_states::Accel_To_Target_Reverse;
		if (Spin::Output::Controls.direction != Spin::Enums::e_directions::Reverse)
		{
			//but we currently arent going reverse, change to reverse direction
			Spin::Output::set_direction(Spin::Enums::e_directions::Reverse);
			//since we toggled directions, we should reset the integral windup
			Spin::ClosedLoop::Pid::Reset_integral();
		}
		actual += spindle_encoder.ticks_per_rev;
	}
	////If state says we need to go forward....
	//if (State == Spin::Enums::e_position_states::Accel_To_Target_Forward
	//	&& Spin::Output::Controls.direction != Spin::Enums::e_directions::Reverse)
	//{
	//	//but we currently arent going forward, change to forward direction
	//	Spin::Output::set_direction(Spin::Enums::e_directions::Reverse);
	//	//since we toggled directions, we should reset the integral windup
	//	Spin::ClosedLoop::Pid::Reset_integral();
	//}
	////If state says we need to go reverse....
	//else if (State == Spin::Enums::e_position_states::Accel_To_Target_Reverse
	//	&& Spin::Output::Controls.direction != Spin::Enums::e_directions::Forward)
	//{
	//	//but we currently arent going reverse, change to reverse direction
	//	Spin::Output::set_direction(Spin::Enums::e_directions::Forward);
	//	//since we toggled directions, we should reset the integral windup
	//	Spin::ClosedLoop::Pid::Reset_integral();
	//}
	////If state says we are on target...
	//else if (State == Spin::Enums::e_position_states::On_Target)
	//{
	//	//we set output to zero.
	//	Spin::ClosedLoop::Pid::output = 0;
	//	//since we toggled directions, we should reset the integral windup
	//	Spin::ClosedLoop::Pid::Reset_integral();
	//}

	//Do we want to hold EXACTLY the requested position?
	/*if (!_check_tolerance(abs(target - actual)))
		return;*/

	Spin::ClosedLoop::Pid::Calculate(target, actual);



	pwm_out = Spin::ClosedLoop::Pid::output;

	//Send the output value to the hardware
	HardwareAbstractionLayer::Outputs::update_output(pwm_out);
}

bool Spin::ClosedLoop::Position::_check_range(int32_t target_position)
{
	//see if we are being requested to go further than the encoder allows
	if ((target_position > spindle_encoder.ticks_per_rev)
		|| (target_position < -spindle_encoder.ticks_per_rev))
		return false;//<--ignore a value that is too high


	return true;//<-- value is 'in range'
}

bool Spin::ClosedLoop::Position::_check_tolerance(int32_t target_position)
{
	//see if we are being requested to adjust a value that is still within tolerance
	if ((Spin::Configuration::User_Settings.Motor_Position_Error != -1)
		&& (target_position < Spin::Configuration::User_Settings.Motor_Position_Error))
		return false;//<--ignore a value that is within tolerance


	return true;//<-- value is out of tolerance
}

void Spin::ClosedLoop::Position::_set_state(int32_t target, int32_t actual)
{
	//Set states
	if ((target - Spin::Configuration::User_Settings.Motor_Position_Error) > actual)
	{
		State = Spin::Enums::e_position_states::Accel_To_Target_Forward;
	}
	else if ((target + Spin::Configuration::User_Settings.Motor_Position_Error) < actual)
	{
		State = Spin::Enums::e_position_states::Accel_To_Target_Reverse;
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
