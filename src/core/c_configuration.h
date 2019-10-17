/*
* c_configuration.h
*
* Created: 10/6/2019 7:01:55 PM
* Author: Family
*/


#ifndef __C_CONFIGURATION_H__
#define __C_CONFIGURATION_H__

#include <stdint.h>

#include "c_enumerations.h"
#define INPUT_NUM_SIZE 10

/*
ENC_FAL - failure message
ENC_PAS - encoder detect message
*/
namespace Spin
{
	class Configuration
	{
		//variables
		public:
		struct s_pid_factors
		{
			int32_t Kp;
			int32_t Ki;
			int32_t Kd;
			int16_t Allowed_Error_Percent;
		};
		struct s_pid_tunings
		{
			s_pid_factors Position;
			s_pid_factors Velocity;
			s_pid_factors Torque;
			
		};
		struct s_encoder
		{
			uint16_t Encoder_Ticks_Per_Rev;/*<------- How many pulses the encoder will produce in one rotation.
			This is NOT the CPR value for a quadrature encoder. If
			ticks per rev is 100 then the CPRR value will be 400 in
			quadrature mode.*/
			Enums::e_encoder_modes Encoder_Mode;/*<-- The mode the encoder will run in. Either simple counter,
			quadrature mode, or quadrature with index*/
			void (*call_vect_a)();/*<---------------- There are several encoder types to choose from. Since I*/
			void (*call_vect_b)();                 /* will not know which type it is at compile time, I can */
			void (*call_vect_z)();                 /* assign a function pointer that the interrupt can call
			for the specific encoder type and avoid a large if or
			switch statement*/
		};

		struct s_drive_settings
		{
			uint8_t Hard_Stop_On_Disable;/*<----------- If the control is disabled, this determines of it coasts
			to a stop or not*/
			uint8_t Soft_Direction_Changes;/*<--------- When a direction change is commanded, this determines
			if it comes to a soft stop before changing directions*/
			uint8_t Drive_Output_Inverted;/*<-----------If inverted 0=100 % on, max pwm=0% on. Otherwise 0=0% on
			max pwm = 100% on*/
			uint32_t Max_PWM_Output;/*<---------------- Highest value the PID can output*/
			uint32_t Drive_Turn_Off_Value;/*<---------- Either 0 or max pwm, depending on inversion*/
			uint32_t Drive_Min_On_Value;/*<-------- 0 may be off, but 1, may not cause the drive to move. output
			may have to raise to 8,10,13 or higher to actually cause the
			motor to rotate*/
			
			s_encoder Encoder_Config;
		};
		struct s_user_settings
		{
			int32_t Motor_Max_RPM;/*<------------------ Drive will not allow the motor to exceed this rpm.
			-1 value indicates there is no limit*/
			int32_t Motor_Min_RPM;/*<------------------ Drive will not allow the motor to go below this rpm.
			-1 value indicates there is no limit*/
			uint16_t Home_Position;/*<----------------- The position the motor should always go to when stopped*/
			uint8_t Tool_Orientation;/*<--------------- Position required for tool changing. Probably should be
			controlled by the motion control though. Im leaving it in
			for now.*/
			Enums::e_directions Default_Direction;/*<-- May remove this. Should always follow the inputs*/

		};

		static s_pid_tunings PID_Tuning;
		static s_drive_settings Drive_Settings;
		static s_user_settings User_Settings;
		static Spin::Enums::e_config_results Status;

		protected:
		private:

		//functions
		public:
		static void initiailize();
		static void load_defaults();
		static Spin::Enums::e_config_results load();
		static void save();
		static void auto_config(char * mdoe);
		static Spin::Enums::e_config_results _config_encoder();
		static void write_message(char * message);
		static char* input_char();
		static int32_t input_int32(char *prompt);
		static uint32_t input_uint32(char * message);
		protected:
		private:

	}; //c_configuration
};
#endif //__C_CONFIGURATION_H__
