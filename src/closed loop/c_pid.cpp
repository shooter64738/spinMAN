/*
* c_pid.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

#include "c_pid.h"
static Spin::Configuration::s_pid_factors factors;
static Spin::Configuration::s_pid_factors terms;
struct s_errors
{
	int32_t process;
	int32_t previous;
	int32_t accumulated;
	int32_t max_process;
	int32_t max_accumulated;
	int8_t direction;

};

static s_errors errors;
int32_t Spin::ClosedLoop::Pid::output = 0;

void Spin::ClosedLoop::Pid::Set_Factors(Spin::Configuration::s_pid_factors init_factors)
{

	factors.Kp = init_factors.Kd * PID_SCALING_FACTOR;
	factors.Ki = init_factors.Ki * PID_SCALING_FACTOR;
	factors.Kd = init_factors.Kd * PID_SCALING_FACTOR;

	errors.max_process = INT16_MAX / (factors.Kp + 1);
	errors.max_accumulated = MAX_I_TERM / (factors.Ki + 1);

	Reset();
}

void Spin::ClosedLoop::Pid::Reset_integral()
{
	errors.accumulated = 0;
}

void Spin::ClosedLoop::Pid::Calculate(int32_t setPoint, int32_t processValue)
{
	
	errors.process = setPoint - processValue;
	errors.direction = (errors.direction < 0) ? -1 : 1;

	Spin::ClosedLoop::Pid::_set_p_term();//<--calculate p term from error and p factor
	Spin::ClosedLoop::Pid::_set_i_term();//<--calculate i term from i factor and accumulated error
	Spin::ClosedLoop::Pid::_set_d_term();//<--calculate d term from d factor and current error
	
	output = ((terms.Kp + terms.Ki + terms.Kd) / PID_SCALING_FACTOR);
	_clamp_output();//<--gold output between the PID_MAX and PID_MIN values.
}

void Spin::ClosedLoop::Pid::Reset()
{

}

void Spin::ClosedLoop::Pid::_clamp_output()
{
	if (output > PID_MAX)
	{
		output = PID_MAX;
	}
	else if (output < PID_MIN)
	{
		output = PID_MIN;
	}
	
}

void Spin::ClosedLoop::Pid::_set_p_term()
{
	// Calculate Pterm and limit error overflow
	if (errors.process > errors.max_process) {
		terms.Kp = INT16_MAX;
	}
	else if (errors.process < -errors.max_process) {
		terms.Kp = -INT16_MAX;
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

void Spin::ClosedLoop::Pid::_set_d_term()
{
	// Calculate Dterm
	terms.Kd = (factors.Kd * (errors.process));
}


