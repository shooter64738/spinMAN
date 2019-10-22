/*
* c_pid.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

//64178 minimum turn on value.
#include "c_pid.h"
#include <math.h>

static Spin::Configuration::s_pid_factors factors;
static Spin::Configuration::s_pid_factors terms;
Spin::Configuration::s_pid_factors *Spin::ClosedLoop::Pid::active_factors;
float Spin::ClosedLoop::Pid::scaler;

Spin::ClosedLoop::Pid::s_errors Spin::ClosedLoop::Pid::errors;
static int32_t old_process_value = 0;

int32_t Spin::ClosedLoop::Pid::output = 0;
int32_t Spin::ClosedLoop::Pid::raw_output = 0;


void Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::s_pid_factors init_factors)
{

	factors.Kp = init_factors.Kp * PID_SCALING_FACTOR;
	factors.Ki = init_factors.Ki * PID_SCALING_FACTOR;
	factors.Kd = init_factors.Kd * PID_SCALING_FACTOR;

	errors.max_process = INT16_MAX;// / (factors.Kp + 1);
	errors.max_accumulated = MAX_I_TERM / (factors.Ki + 1);

	Restart();
}

void Spin::ClosedLoop::Pid::Load_Factors_For_Mode(Spin::Enums::e_drive_modes Mode)
{
	if (Mode == Spin::Enums::e_drive_modes::Velocity)
	{
		Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Velocity);
	}
	else if (Mode == Spin::Enums::e_drive_modes::Position)
	{
		Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Position);
	}
	else if (Mode == Spin::Enums::e_drive_modes::Torque)
	{
		Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::PID_Tuning.Torque);
	}
}


void Spin::ClosedLoop::Pid::Reset_integral()
{
	errors.accumulated = 0;
}

void Spin::ClosedLoop::Pid::Calculate(int32_t processValue)
{
	//Assume the error value was set external to pid, and we use that
	
	_internal_pid_comp(processValue);
}

void Spin::ClosedLoop::Pid::Calculate(int32_t setPoint, int32_t processValue)
{
	//Assume the error value set externally should NOT be used. Get our own internal value.
	_internal_error_comp(setPoint, processValue);
	_internal_pid_comp(processValue);
}

void Spin::ClosedLoop::Pid::Reset()
{
	//reset factors
	factors.Kd = 0;
	factors.Ki = 0;
	factors.Kd = 0;

	//reset max values
	errors.max_process = 0;
	errors.max_accumulated = 0;

	//reset terms, errors, outputs
	Spin::ClosedLoop::Pid::Restart();

}

void Spin::ClosedLoop::Pid::Restart()
{

	//reset terms
	terms.Kp = 0;
	terms.Ki = 0;
	terms.Kd = 0;
	//reset errors
	errors.process = 0;
	errors.previous = 0;
	errors.accumulated = 0;

	errors.direction = 0;

	old_process_value = 0;
	output = 0;
}

void Spin::ClosedLoop::Pid::_internal_pid_comp(int32_t processValue)
{
	Spin::ClosedLoop::Pid::_set_p_term();//<--calculate p term from error and p factor
	Spin::ClosedLoop::Pid::_set_i_term();//<--calculate i term from i factor and accumulated error
	Spin::ClosedLoop::Pid::_set_d_term(processValue);//<--calculate d term from d factor and current error

	float f_p = terms.Kp*scaler;
	float f_i = terms.Ki*scaler;
	float f_d = terms.Kd*scaler;

	Spin::ClosedLoop::Pid::raw_output = abs(((f_p)+(f_i)+(f_d)) / PID_SCALING_FACTOR);
	Spin::ClosedLoop::Pid::output = Spin::ClosedLoop::Pid::raw_output;

	_clamp_output();//<--gold output between the PID_MAX and PID_MIN values.
	old_process_value = processValue;
}

void Spin::ClosedLoop::Pid::_internal_error_comp(int32_t setPoint, int32_t processValue)
{
	errors.process = (setPoint - processValue);
	errors.direction = (errors.process < 0) ? -1 : 1;
}

void Spin::ClosedLoop::Pid::_clamp_output()
{
	//Make sure the out value doesnt exceed hardware
	if (output > PID_MAX)
	{
		output = PID_MAX;
		//If we have reached output saturation, limit integral windup
		errors.accumulated -= (errors.process);
	}
	else if (output < PID_MIN)
	{
		output = PID_MIN;
		//If we have reached output saturation, limit integral windup
		errors.accumulated -= (errors.process);
	}

	/*
	lag_correction is used to determine the 'jump' value for the pid output.
	If 65535 is totally off, and the motor doesnt react until 64000, that is
	a lag. In velocity mode its just a delay for the motor to start, but in
	position mode it will cause a lag in the reaction of the drive to get
	to its intended target point. It will appear as an oscillation even if the
	pid algorithm is tuned perfectly.
	*/

	//Do we need to invert the output?
	if (Spin::Configuration::Drive_Settings.Drive_Output_Inverted)
	{
		output = Spin::Configuration::Drive_Settings.Max_PWM_Output - Spin::ClosedLoop::Pid::output;
		//adjust for lag
		output -= (PID_MAX - Spin::Configuration::Drive_Settings.Drive_Min_On_Value);
		if (output < PID_MIN) output = PID_MIN;
	}
	else
	{
		//adjust for lag
		output += Spin::Configuration::Drive_Settings.Drive_Min_On_Value;
		if (output > PID_MAX) output = PID_MAX;
	}

}

void Spin::ClosedLoop::Pid::_set_p_term()
{
	// Calculate Pterm and limit error overflow
	if (errors.process > (errors.max_process)) {
		terms.Kp = (INT16_MAX);
	}
	else if (errors.process < -(errors.max_process)) {
		terms.Kp = -(INT16_MAX);
	}
	else {
		terms.Kp = factors.Kp * errors.process;
	}
}

void Spin::ClosedLoop::Pid::_set_i_term()
{
	int32_t temp = 0;

	// Calculate Iterm and limit integral runaway
	temp = errors.accumulated + (errors.process);
	if (temp > errors.max_accumulated) {
		terms.Ki = MAX_I_TERM;
		errors.accumulated = errors.max_accumulated;
	}
	else if (temp < -errors.max_accumulated) {
		terms.Ki = -MAX_I_TERM;
		errors.accumulated = -errors.max_accumulated;
	}
	else {
		errors.accumulated = temp;
		terms.Ki = (factors.Ki * errors.accumulated);
	}
}

void Spin::ClosedLoop::Pid::_set_d_term(int32_t current_process_value)
{
	// Calculate Dterm
	terms.Kd = (factors.Kd * (old_process_value - current_process_value));
}


