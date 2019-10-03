/*
* PID.h
*
* Created: 8/11/2018 7:24:55 PM
* Author: Family
*/


#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>

#define MINIMUM_OUTPUT 0
#define MAXIMUM_OUTPUT 255

class c_PID
{
	typedef struct  
	{
		double Kp;
		double Ki;
		double Kd;
		
		double KITerm;
		double lastInput;
		double NewOutPut;
		double Err;
		uint8_t Max_Val;
		uint8_t Min_Val;
	}s_pid_terms;
	//variables
	public:
	
	static s_pid_terms servo_terms;
	static s_pid_terms spindle_terms;
	


	//functions
	public:
	static void Initialize();
	static void Clear(s_pid_terms &terms);
	static uint8_t Calculate(float current, float target, s_pid_terms &terms);
	static uint8_t Calculate(int32_t current, int32_t target, s_pid_terms &terms);
	protected:
	private:
	
	c_PID();
	~c_PID();
	c_PID( const c_PID &c );
	c_PID& operator=( const c_PID &c );

}; //PID

#endif //__PID_H__
