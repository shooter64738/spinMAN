/*
* c_configuration.cpp
*
* Created: 10/6/2019 7:01:54 PM
* Author: Family
*/


#include "c_configuration.h"
#include "../bit_manipulation.h"
#include "../driver/c_input.h"
#include "../driver/c_controller.h"
#include "../driver/volatile_encoder_externs.h"
#include "../driver/volatile_input_externs.h"
#include "../closed loop/c_velocity.h"
#include <math.h>

Spin::Configuration::s_pid_tunings Spin::Configuration::PID_Tuning;
Spin::Configuration::s_drive_settings Spin::Configuration::Drive_Settings;
Spin::Configuration::s_user_settings Spin::Configuration::User_Settings;
Spin::Configuration::s_work_data Spin::Configuration::Working_Data;
Spin::Enums::e_config_results Spin::Configuration::Status;

void Spin::Configuration::initiailize()
{
	//Default max output to 255;
	Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT8_MAX;
	/*
	Try to get the maximum output the timer can have.
	*/
	switch (PWM_RESOLUTION_BIT)
	{
		case 32:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT32_MAX;
			break;
		}
		case 16:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT16_MAX;
			break;
		}
		case 8:
		{
			Spin::Configuration::Drive_Settings.Max_PWM_Output = UINT8_MAX;
			break;
		}
	}
}

void Spin::Configuration::load_defaults()
{
	//If defaults need to be re-loaded due to bad config saved, we need to re-init
	Spin::Configuration::initiailize();

	//Some default pid values
	Spin::Configuration::PID_Tuning.Position.Kp = 10;
	Spin::Configuration::PID_Tuning.Position.Ki = 5;
	Spin::Configuration::PID_Tuning.Position.Kd = 2;

	Spin::Configuration::PID_Tuning.Velocity.Kp = 4;
	Spin::Configuration::PID_Tuning.Velocity.Ki = 4;
	Spin::Configuration::PID_Tuning.Velocity.Kd = 1;

	Spin::Configuration::PID_Tuning.Torque.Kp = 0;
	Spin::Configuration::PID_Tuning.Torque.Ki = 0;
	Spin::Configuration::PID_Tuning.Torque.Kd = 0;

	Spin::Configuration::Drive_Settings.Drive_Output_Inverted = 1;
	Spin::Configuration::Drive_Settings.Hard_Stop_On_Disable = 1;

	//Default to either 0 or highest pwm value, depending on inversion
	Spin::Configuration::Drive_Settings.Drive_Turn_Off_Value
	= Drive_Settings.Drive_Output_Inverted ? Spin::Configuration::Drive_Settings.Max_PWM_Output : 0;

	//Default to either 0 or highest pwm value, depending on inversion. This may change when user config is loaded
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value
	= Drive_Settings.Drive_Output_Inverted ? Spin::Configuration::Drive_Settings.Max_PWM_Output : 0;

	Spin::Configuration::User_Settings.Motor_Max_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Motor_Min_RPM = -1; //default to no limit
	Spin::Configuration::User_Settings.Motor_RPM_Error = 5; //default to +/-5rpm
	Spin::Configuration::User_Settings.Motor_Position_Error = 5;
	Spin::Configuration::User_Settings.Motor_Accel_Rate_Per_Second = 150;//default to 150rpm/sec/sec
	Spin::Configuration::User_Settings.Home_Position = 0; //default to 0
	Spin::Configuration::User_Settings.Tool_Orientation = 0; //default to 0
	Spin::Configuration::User_Settings.Default_Direction = Enums::e_directions::Free;//set to free spin.
	
	Spin::Configuration::Drive_Settings.Drive_Min_On_Value = 64188;
	//Encoder has 100 pulses in a rotation.
	//If quadrature mode is active the 100 pulses per rotation is multiplied by 4 (quadrature count)
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value = 100;
	Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = Enums::e_encoder_modes::Quadrature;
	
	
	//assume there has not been a config done.
	Spin::Configuration::Status = Spin::Enums::e_config_results::Incomplete_Config;

	Spin::Configuration::set_working_data();
	
}

Spin::Enums::e_config_results Spin::Configuration::load()
{
	uint8_t key;
	uint8_t pos = 0;
	//Convert pid settings to char*
	char _stream[(sizeof(s_pid_tunings)+sizeof(s_drive_settings)+sizeof(s_user_settings)+sizeof(uint8_t))];
	HardwareAbstractionLayer::Storage::load(_stream, sizeof(_stream));

	memcpy(&key, _stream + pos, sizeof(uint8_t));
	pos += sizeof(uint8_t);
	memcpy(&PID_Tuning, _stream + pos, sizeof(s_pid_tunings));
	pos += sizeof(s_pid_tunings);
	memcpy(&Drive_Settings, _stream + pos, sizeof(s_drive_settings));
	pos += sizeof(s_drive_settings);
	memcpy(&User_Settings, _stream + +pos, sizeof(s_user_settings));

	Spin::Configuration::set_working_data();
	
	//If there are no pointers for the rpm vectors then reload defaults
	//and indicate this config is invalid. It is either new or corrupted
	if (key != 001)
	{
		load_defaults();
		Spin::Driver::Controller::host_serial.print_string("RUNNING DEFAULTS!\r\n");
		return Enums::e_config_results::Encoder_Not_Available;
	}
	return Enums::e_config_results::Encoder_Available;
}

void Spin::Configuration::save()
{
	uint8_t key = 222;
	uint8_t pos = 0;
	//Convert pid settings to char*
	char stream[(sizeof(s_pid_tunings)+sizeof(s_drive_settings)+sizeof(s_user_settings)+sizeof(uint8_t))];
	
	memcpy(stream, &key, sizeof(uint8_t));
	pos += sizeof(uint8_t);
	memcpy(stream + pos, &PID_Tuning, sizeof(s_pid_tunings));
	pos += sizeof(s_pid_tunings);
	memcpy(stream + pos, &Drive_Settings, sizeof(s_drive_settings));
	pos += sizeof(s_drive_settings);
	memcpy(stream + pos, &User_Settings, sizeof(s_user_settings));
	
	HardwareAbstractionLayer::Storage::save(stream, sizeof(stream));


}

Spin::Enums::e_config_results Spin::Configuration::set_working_data()
{
	Spin::Enums::e_config_results result = Spin::Enums::e_config_results::Encoder_Available;

	Spin::ClosedLoop::Velocity::Acceleration_Per_Cycle = pow((Spin::Configuration::User_Settings.Motor_Accel_Rate_Per_Second * (1 / PID_PERIODS_IN_INTERVAL)), 2);

	result = Spin::Configuration::_assign_encoder_vectors(Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Mode);
	if (result != Spin::Enums::e_config_results::Configuration_Ok)
		return result;
	
	
}

Spin::Enums::e_config_results Spin::Configuration::_assign_encoder_vectors(Enums::e_encoder_modes encoder_type)
{
	//We need the ppr value or the encoder is useless
	if (Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value < 1)
	return Enums::e_config_results::No_Encoder_PPR_Set;
	
	switch (encoder_type)
	{
		case Spin::Enums::e_encoder_modes::No_Encoder:
		{
			//no encoder found, effectively we are doing nothing
			
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.no_vect;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.no_vect;

			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev = 0;
			spindle_encoder.half_ticks_per_rev = 0;
			
			break;
		}
		case Spin::Enums::e_encoder_modes::Index_Only:
		{
			//we only have an index pulse. 
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.read_chz;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.get_rpm_index;

			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev = 0;
			spindle_encoder.half_ticks_per_rev = 0;
			
			break;
		}
		case Spin::Enums::e_encoder_modes::Quadrature:
		{
			//assign function pointers to what needs to be called for this encoder type.
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.read_quad;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_quad;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.no_vect;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.get_rpm_quad;
			
			//configure the hardware to read a quadrature WITHOUT an index pulse.
			HardwareAbstractionLayer::Encoder::Configurator.configure_encoder_quadrature();
			
			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev
			= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value * 4;
			spindle_encoder.half_ticks_per_rev
				= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value * 2;
			
			break;
		}
		case Spin::Enums::e_encoder_modes::Quadrature_wIndex:
		{
			//assign function pointers to what needs to be called for this encoder type.
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.read_quad_with_z;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_quad_with_z;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.read_chz;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.get_rpm_quad;
			
			//configure the hardware to read a quadrature WITHOUT an index pulse.
			Spin::Driver::Controller::host_serial.print_string("QUAD CONFIGURE\r\n");
			HardwareAbstractionLayer::Encoder::Configurator.configure_encoder_quadrature_w_z();
			
			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev
			= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value * 4;
			spindle_encoder.half_ticks_per_rev
				= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value * 2;

			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanA:
		{
			//assign function pointers to what needs to be called for this encoder type.
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.read_cha;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.no_vect;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.get_rpm_single_ch;
			
			//configure the hardware to read a quadrature WITHOUT an index pulse.
			HardwareAbstractionLayer::Encoder::Configurator.config_cha();
			
			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev
			= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value;

			spindle_encoder.half_ticks_per_rev
				= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value / 2;
			break;
		}
		case Spin::Enums::e_encoder_modes::Simple_ChanB:
		{
			//assign function pointers to what needs to be called for this encoder type.
			spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.no_vect;
			spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_chb;
			spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.no_vect;

			//assign the appropriate function to calculate rpm for quadrature WITHOUT index
			spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::Calculator.get_rpm_single_ch;
			
			//configure the hardware to read a quadrature WITHOUT an index pulse.
			HardwareAbstractionLayer::Encoder::Configurator.config_chb();
			
			//Since it is quadrature multiply ppr times 4 to get cpr.
			spindle_encoder.ticks_per_rev
			= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value;

			spindle_encoder.half_ticks_per_rev
				= Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_PPR_Value / 2;
			break;
		}
		default:
		{
			Spin::Driver::Controller::host_serial.print_string("unknown encoder type!\r\n");
			break;
		}
		
	}
	HardwareAbstractionLayer::Encoder::initialize();

	return Spin::Enums::e_config_results::Configuration_Ok;
}
//
//void Spin::Configuration::auto_config(char * config_type)
//{
	//Spin::Configuration::_config_encoder();
//}
//
//Spin::Enums::e_config_results Spin::Configuration::_config_encoder()
//{
	//write_message("ENC_CFG:***Stand clear of motor!*** Press Enter to Start\r\n\0");
	////disable the drive
	//HardwareAbstractionLayer::Outputs::disable_output();
	////configure the encoder for all signals
	//HardwareAbstractionLayer::Encoder::Configurator.configure_test_channels();
		//
	////Set output to half. 0 or max val might make the motor spin waaay to fast all at once.
	//uint32_t current_output = Spin::Configuration::Drive_Settings.Max_PWM_Output/2;
	//
	////set output to current
	//HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
	//HardwareAbstractionLayer::Outputs::enable_output();
	//HardwareAbstractionLayer::Outputs::update_output(current_output);
	//
	//spindle_encoder.active_channels = 0;
	//for (int i=0;i<6;i++)
	////while(1)
	//{
		//while (!BitTst(extern_input__intervals, ONE_INTERVAL_BIT))
		//{
			///*
			//since quadrature with index is all the features we can support, if we
			//detect that encoder type, we can stop this loop early.
			//*/
			//HardwareAbstractionLayer::Encoder::Reader.read_active_channels();
			//
			//if (spindle_encoder.active_channels == (int)Enums::e_encoder_modes::Quadrature_wIndex)
			//break;
		//}
		//extern_input__intervals &= ~(1<<ONE_INTERVAL_BIT);
		//Spin::Driver::Controller::host_serial.print_string("=");
		////extern_encoder__active_channels = 0;
	//}
	//Spin::Driver::Controller::host_serial.print_string(">\r");
	//if (spindle_encoder.active_channels == 0 )
	//{
		//write_message("ENC_NON\r\n\0");
		//return Spin::Enums::e_config_results::Encoder_Not_Available ;
	//}
	////stop the drive
	//HardwareAbstractionLayer::Outputs::disable_output();
	//HardwareAbstractionLayer::Outputs::update_output(0);
	//
	//
	///*
	//active channels is a bit flag value
	//binar value					decimal
	//00000000 = no encoder		0
	//00000001 = active chan a	1
	//00000010 = active chan b	2
	//00000011 = active chan ab	3
	//00000100 = active chan z	4
	//00000101 = active chan za	5
	//00000110 = active chan zb	6
	//00000111 = active chan zba	7
	//
	//therefore in decimal value it will match the enum values
	//*/
	//Configuration::Drive_Settings.Encoder_Config.Encoder_Mode = (Spin::Enums::e_encoder_modes) spindle_encoder.active_channels;
	////Now that the encoder type is known we can set the pointer for the isrs to call when something happens
	//Spin::Configuration::_assign_encoder_vectors(Configuration::Drive_Settings.Encoder_Config.Encoder_Mode);
//
	////now ask the user for the PPR value.
	//Drive_Settings.Encoder_Config.Encoder_PPR_Value = 0;
	//while (Drive_Settings.Encoder_Config.Encoder_PPR_Value == 0)
	//{
		//Drive_Settings.Encoder_Config.Encoder_PPR_Value = input_int32("ENC_PPR?\r\n\0");
		//if (Drive_Settings.Encoder_Config.Encoder_PPR_Value <= 0)
		//{
			//write_message("ENC_FAL\r\n\0");
			//Drive_Settings.Encoder_Config.Encoder_PPR_Value = 0;
			//return Spin::Enums::e_config_results::Encoder_Not_Available;
		//}
		//break;
	//}
//
	//Spin::Driver::Controller::host_serial.print_string("ENC_CHN:");
	////This will cover most encoders, but I handle quadrature and quadrature with index different
	//if (spindle_encoder.active_channels & ENC_CHA_TRK_BIT)
	//{
		//spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.read_cha;
		//Spin::Driver::Controller::host_serial.print_string("A");
	//}
	//if (spindle_encoder.active_channels & ENC_CHB_TRK_BIT)
	//{
		//spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_chb;
		//Spin::Driver::Controller::host_serial.print_string("B");
	//}
	//if (spindle_encoder.active_channels & ENC_CHZ_TRK_BIT)
	//{
		//spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::Reader.read_chz;
		//Spin::Driver::Controller::host_serial.print_string("Z");
	//}
	////Special handler for quadrature because they report direction too.
	//if (Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature
	//|| Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature_wIndex)
	//{
		//Spin::Driver::Controller::host_serial.print_string(" QD");
		//spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::Reader.read_quad;
		//spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_quad;
		//if (Configuration::Drive_Settings.Encoder_Config.Encoder_Mode == Spin::Enums::e_encoder_modes::Quadrature_wIndex)
		//{
			//spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::Reader.read_chz;
			//Spin::Driver::Controller::host_serial.print_string(" I");
		//}
	//}
	//Spin::Driver::Controller::host_serial.print_string("\r\n");
	//
	//
	////Here we need to run a forward/reverse test to determine the proper direction of the encoder
	//
	////Clear the encoder position
	//spindle_encoder.position = 0;
	//Spin::Driver::Controller::host_serial.print_string("ENC_DIR\r");
	////extern_encoder__direction = 0;
	//
	////set output to current
	//HardwareAbstractionLayer::Outputs::enable_output();
	//HardwareAbstractionLayer::Outputs::update_output(current_output);
	//HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
	//
	//for (int i=0;i<6;i++)
	////while(1)
	//{
		//while (!BitTst(extern_input__intervals, ONE_INTERVAL_BIT))
		//{
			///*
			//since quadrature with index is all the features we can support, if we
			//detect that encoder type, we can stop this loop early.
			//*/
			//HardwareAbstractionLayer::Encoder::Reader.read_active_channels();
			//
			//if (spindle_encoder.direction != 0)
			//break;
		//}
		//extern_input__intervals &= ~(1<<ONE_INTERVAL_BIT);
		//Spin::Driver::Controller::host_serial.print_string("=");
		////extern_encoder__active_channels = 0;
	//}
	//HardwareAbstractionLayer::Outputs::disable_output();
	//Spin::Driver::Controller::host_serial.print_string(">\r");
	//Spin::Driver::Controller::host_serial.print_string("ENC_DIR:");
	//Spin::Driver::Controller::host_serial.print_int32(spindle_encoder.direction);
	//Spin::Driver::Controller::host_serial.print_string("DIR_FWD\r");
	//write_message("ENC_OK\r\n\0");
	//
	//
	//
	//write_message("ENC_OK\r\n\0");
	//return Spin::Enums::e_config_results::Encoder_Available;
//}
//
//void Spin::Configuration::write_message(char * message)
//{
	//Spin::Driver::Controller::host_serial.print_string(message);
	//while(!Spin::Driver::Controller::host_serial.HasEOL())
	//{
	//}
	//Spin::Driver::Controller::host_serial.SkipToEOL();
	//Spin::Driver::Controller::host_serial.Reset();
//}
//
//char* Spin::Configuration::input_char()
//{
	//return NULL;
//}
//
//int32_t Spin::Configuration::input_int32(char * message)
//{
	//write_message(message);
	//while(!Spin::Driver::Controller::host_serial.HasEOL())
	//{
		//if (Spin::Driver::Controller::host_serial.HasEOL())
		//{
			//char  num[INPUT_NUM_SIZE];
			//char * _num;
			//_num = num;
			//memcpy(num,0,sizeof(int32_t));
			//num[0] = Spin::Driver::Controller::host_serial.Get();
			//
			//for (int i = 1; i < INPUT_NUM_SIZE; i++)
			//{
				//if (Spin::Driver::Controller::host_serial.Peek() == 13
				//|| Spin::Driver::Controller::host_serial.Peek() == 10)
				//{
					//Spin::Driver::Controller::host_serial.SkipToEOL();
					//Spin::Driver::Controller::host_serial.Reset();
					//break; // break the for loop
				//}
				//if (Spin::Driver::Controller::host_serial.Peek() == '\\') //user canceled input
				//return -1;
				//num[i] = Spin::Driver::Controller::host_serial.Get();
			//}
			//int32_t _var = 0;
			//_var = atoi(_num);
			//return _var;
		//}
	//}
	//return 0;
//}
//
//uint32_t Spin::Configuration::input_uint32(char * message)
//{
	//return 0;
//}