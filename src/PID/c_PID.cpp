/*
* PID.cpp
*
* Created: 8/11/2018 7:24:55 PM
* Author: Family
*/


#include "c_PID.h"
#include "..\c_Processor.h"
#include "..\Time\c_Time_Keeper.h"

c_PID::s_pid_terms c_PID::servo_terms;
c_PID::s_pid_terms c_PID::spindle_terms;

void c_PID::Initialize()
{
	c_PID::servo_terms.Kp=0.0015;
	c_PID::servo_terms.Ki=0.0001;
	c_PID::servo_terms.Kd=-0.01;
	c_PID::servo_terms.KITerm=0;
	c_PID::servo_terms.lastInput = 0;
	c_PID::servo_terms.NewOutPut = 0;
	c_PID::servo_terms.Err=0;
	c_PID::servo_terms.Max_Val=255;
	c_PID::servo_terms.Min_Val=-255;
	
	//c_PID::spindle_terms.Kp=0.05;
	//c_PID::spindle_terms.Ki=0.0011;
	//c_PID::spindle_terms.Kd=0.0001;
	c_PID::spindle_terms.Kp=0.0015;
	c_PID::spindle_terms.Ki=0.0001;
	c_PID::spindle_terms.Kd=0.01;
	c_PID::spindle_terms.KITerm=0;
	c_PID::spindle_terms.lastInput = 0;
	c_PID::spindle_terms.NewOutPut = 0;
	c_PID::spindle_terms.Err=0;
	c_PID::spindle_terms.Max_Val=255;
	c_PID::spindle_terms.Min_Val=100;
	
	//c_Spindle_Drive::current_output = MINIMUM_OUTPUT;
	
}

void c_PID::Clear(s_pid_terms &terms)
{
	terms.KITerm=0;
	terms.lastInput = 0;
	terms.NewOutPut = terms.Min_Val;
	terms.Err=0;
}
uint32_t last_millis = 0;
uint8_t c_PID::Calculate(int32_t current, int32_t target, s_pid_terms &terms)
{
	
	double error = target - current;
	terms.KITerm+= (terms.Ki * error);
	if(terms.KITerm > terms.Max_Val) terms.KITerm= terms.Max_Val;
	else if(terms.KITerm < terms.Min_Val) terms.KITerm= terms.Min_Val;
	
	double dInput = (current - target);
	
	/*Compute PID Output*/
	terms.NewOutPut = terms.Kp * error + terms.KITerm - terms.Kd * dInput;
	
	if(terms.NewOutPut > terms.Max_Val) terms.NewOutPut = terms.Max_Val;
	else if(terms.NewOutPut < terms.Min_Val) terms.NewOutPut = terms.Min_Val;
	
	return terms.NewOutPut;
	
	//double error = target - current;
	//terms.KITerm += error;       //this is the integral part which eliminates the steady state error
	//if(terms.KITerm > terms.Max_Val)  //here we limit the integral part
	//terms.KITerm = terms.Max_Val;
	//if(terms.KITerm < terms.Min_Val)
	//terms.KITerm = terms.Min_Val;
	//
	////this can also be written like this
	//double output = terms.Kp*error + terms.Kd*(error - terms.Err) + terms.Ki*terms.KITerm;
	
	
	terms.Err = target - current;    //update the error
	//return output;
}
uint8_t c_PID::Calculate(float current, float target, s_pid_terms &terms)
{
	double error = target - current;
	terms.KITerm+= (terms.Ki * error);
	if(terms.KITerm > terms.Max_Val) terms.KITerm= terms.Max_Val;
	else if(terms.KITerm < terms.Min_Val) terms.KITerm= terms.Min_Val;
	
	double dInput = (current - target);
	
	/*Compute PID Output*/
	double output = terms.Kp * error + terms.KITerm- terms.Kd * dInput;
	
	if(output > terms.Max_Val) output = terms.Max_Val;
	else if(output < terms.Min_Val) output = terms.Min_Val;
		
	return output;
	
	//double error = target - current;
	//
	////changeError = error - last_error; // derivative term
	//double changeError = error - terms.Err;
	////totalError += error; //accumalate errors to find integral term
	//terms.KITerm+= error;
	/////pidTerm = (Kp * error) + (Ki * totalError) + (Kd * changeError);//total gain
	//double output = (terms.Kp * error) + (terms.Ki * terms.KITerm) + (terms.Kd * changeError);//total gain
	////pidTerm = constrain(pidTerm, -255, 255);//constraining to appropriate value
	//if(output > terms.Max_Val) output = terms.Max_Val;
	//else if(output < terms.Min_Val) output = terms.Min_Val;
	////pidTerm_scaled = abs(pidTerm);//make sure it's a positive value
	////last_error = error;
	//terms.Err = error;
	//return output;
}
// default constructor
c_PID::c_PID()
{
} //PID

// default destructor
c_PID::~c_PID()
{
} //~PID
