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

struct s_correction
{
	Spin::Enums::e_directions Nearest_Direction;
	uint32_t Nearest_Distance;
};

static s_correction Corrections;

void Spin::ClosedLoop::Position::step(int32_t target, int32_t actual)
{
	//if (!_check_range(target))
	//return;
	Spin::ClosedLoop::Pid::scaler = 1.0;
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value = 64500;
	Spin::Configuration::User_Settings.Motor_Position_Error = 10;

	_find_closest_error(target, actual);
	
	_set_state();

	if (State != Spin::Enums::e_position_states::On_Target
	&& Spin::Output::Controls.direction != Corrections.Nearest_Direction)
	{
		Spin::Output::set_direction(Corrections.Nearest_Direction);
		Spin::ClosedLoop::Pid::Reset_integral();
	}
	//If state says we are on target...
	else if (State == Spin::Enums::e_position_states::On_Target)
	{
		//we set output to zero.
		Spin::ClosedLoop::Pid::output = 0;
		//since we toggled directions, we should reset the integral windup
		Spin::ClosedLoop::Pid::Reset_integral();
	}

	//set the error value externally for pid to correct
	Spin::ClosedLoop::Pid::errors.process = Corrections.Nearest_Distance;

	//Do we want to hold EXACTLY the requested position?
	if (!_check_tolerance(Corrections.Nearest_Distance))
	return;

	
	//send in only the actual position.
	Spin::ClosedLoop::Pid::Calculate(actual);

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

void Spin::ClosedLoop::Position::_set_state()
{
	//Nearest distance is always a positive value so we only need to know if its > 0
	if ((Corrections.Nearest_Distance - Spin::Configuration::User_Settings.Motor_Position_Error) > 0)
	{
		if (Corrections.Nearest_Direction == Spin::Enums::e_directions::Forward)
		State = Spin::Enums::e_position_states::Accel_To_Target_Forward;
		else if (Corrections.Nearest_Direction == Spin::Enums::e_directions::Reverse)
		State = Spin::Enums::e_position_states::Accel_To_Target_Reverse;
	}
	else
	{
		//we are on target, do nothing
		State = Spin::Enums::e_position_states::On_Target;
	}
}

int32_t Spin::ClosedLoop::Position::_clamp_acceleration(int32_t target, int32_t actual)
{
	return target;
}
void Spin::ClosedLoop::Position::_find_closest_error(int32_t target, int32_t actual)
{
	uint32_t raw_diff = target > actual ? target - actual : actual - target;
	uint32_t mod_diff = raw_diff % spindle_encoder.ticks_per_rev;
	//uint32_t mod_diff = std::fmodl(raw_diff, spindle_encoder.ticks_per_rev);
	uint32_t dist = mod_diff > spindle_encoder.half_ticks_per_rev ? spindle_encoder.ticks_per_rev - mod_diff : mod_diff;

	if (mod_diff > (spindle_encoder.half_ticks_per_rev)) {
		//There is a shorter path in opposite direction
		Corrections.Nearest_Direction = Spin::Enums::e_directions::Forward;
		if (target > actual)
		Corrections.Nearest_Direction = Spin::Enums::e_directions::Reverse;
	}
	else {
		Corrections.Nearest_Direction = Spin::Enums::e_directions::Forward;
		if (actual > target)
		Corrections.Nearest_Direction = Spin::Enums::e_directions::Reverse;

	}
	Corrections.Nearest_Distance = dist;
}

