/*
* c_pid.cpp
*
* Created: 10/9/2019 9:23:52 AM
* Author: jeff_d
*/

#include "c_velocity.h"
#include "../hardware_def.h"
#include "c_pid.h"

static int16_t pwm_out = 0;

void Spin::ClosedLoop::Velocity::step(int32_t target, int32_t actual)
{
	//First order of business is to get teh pid value
	Spin::ClosedLoop::Pid::Calculate(target, actual);
	
	//Assume we use the pid output as a raw value
	pwm_out = Spin::ClosedLoop::Pid::output;
	
	



	//Send the updated value out to the hardware
	HardwareAbstractionLayer::Outputs::update_output(pwm_out);
	
}
