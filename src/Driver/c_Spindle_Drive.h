/*
* c_Spindle_Drive.h
*
* Created: 8/8/2018 1:35:00 PM
* Author: jeff_d
*/

#include <avr/io.h>
#include <avr/interrupt.h>


#ifndef __C_SPINDLE_DRIVE_H__
#define __C_SPINDLE_DRIVE_H__
#define STATE_BIT_BRAKE 0
#define STATE_BIT_DIRECTION_CW 1
#define STATE_BIT_DIRECTION_CCW 2
#define STATE_BIT_ACCELERATE 3
#define STATE_BIT_DECELERATE 4
#define STATE_BIT_DISABLE 5
#define STATE_BIT_ENABLE 6
#define STATE_BIT_STOPPED 7

class c_Spindle_Drive
{
	typedef struct
	{
		uint16_t State; //<-- current state of the control in bit masking
		float target_rpm;
		float motion_steps_per_encoder_tick;
		uint8_t feed_mode ;
		int8_t direction;
		float buffered_motion_control_pulses;
		uint32_t start_time;
		float accel_time;
		uint32_t encoder_at_state_change;
	} s_drive_control;

	//variables
	public:
	static s_drive_control Drive_Control;
	static double Kp, Ki, Kd, B; //<--setup PID scalers
	static int32_t Target_Position;
	

	protected:
	private:

	//functions
	public:
	static void Initialize();
	static void Accelerate();
	static void Decelerate();
	static void Adjust(uint8_t output_value);
	static void Enable_Drive();
	static void Disable_Drive();
	static void Brake_Drive();
	static void Release_Drive();
	static void Reverse_Drive();
	static void Forward_Drive();
	static void Servo(int32_t position);
	static uint8_t Check_State();
	static void Process_State(float current_rpm);
	static void Set_Acceleration(float current_rpm);
	static void Set_Decelerate(float current_rpm);
	static void Set_State(uint8_t state_bit_flag);
	static uint8_t Get_State(uint8_t state_bit_flag);
	static void Clear_State(uint8_t state_bit_flag);
	static uint8_t PID_Calculate(float current_RPM, float target_RPM, double ki, double kd, double kp);
	static void PID_Clear();

	static uint8_t current_output;
	static uint32_t last_millis;
	

	protected:
	private:
	c_Spindle_Drive( const c_Spindle_Drive &c );
	c_Spindle_Drive& operator=( const c_Spindle_Drive &c );
	c_Spindle_Drive();
	~c_Spindle_Drive();

}; //c_Spindle_Drive

#endif //__C_SPINDLE_DRIVE_H__
