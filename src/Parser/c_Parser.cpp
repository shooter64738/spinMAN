/*
* c_Spindle.cpp
*
* Created: 3/6/2018 1:21:49 PM
* Author: jeff_d
*/


#include "c_Parser.h"
#include "..\c_Processor.h"
#include "..\Driver\c_Spindle_Drive.h"
#include "..\FeedBack\c_Encoder_RPM.h"
#include "..\PID\c_PID.h"
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

char c_Parser::Line[CYCLE_LINE_LENGTH];
c_Parser::s_spindle c_Parser::spindle_input;

void c_Parser::Initialize()
{
	c_Processor::host_serial.print_string("Parser initialized.");
}

int16_t c_Parser::CheckInput()
{
	//$ is for adjusting a setting, anything else should be processed as a command
	
	//Must have a CR before we consider data 'input'
	if (!c_Processor::host_serial.HasEOL()) return NGC_Interpreter_Errors::OK;
	
	uint16_t return_value = NGC_Interpreter_Errors::OK;
	
	//If the input begins with $, its a setting adjustment
	if (c_Processor::host_serial.Peek() == '$')
	{
		
	}
	//If the input begins with anything else, it's command data
	else
	{
		
		c_Processor::host_serial.print_string("processing input");
		return_value = _ParseValues();
		
		c_Processor::host_serial.Write("spindle_input.feed_rate:");
		c_Processor::host_serial.print_float(c_Parser::spindle_input.feed_rate);
		c_Processor::host_serial.Write("spindle_input.roation_angle:");
		c_Processor::host_serial.print_float(c_Parser::spindle_input.roation_angle);
		c_Processor::host_serial.Write("spindle_input.rotation_direction:");
		c_Processor::host_serial.print_float(c_Parser::spindle_input.rotation_direction);
		c_Processor::host_serial.Write("spindle_input.target_rpm:");
		c_Processor::host_serial.print_float(c_Parser::spindle_input.target_rpm);
		
		c_Processor::host_serial.Write("P,I,D");
		c_Processor::host_serial.print_float(c_PID::spindle_terms.Kp);
		c_Processor::host_serial.Write(",");
		c_Processor::host_serial.print_float(c_PID::spindle_terms.Ki);
		c_Processor::host_serial.Write(",");
		c_Processor::host_serial.print_float(c_PID::spindle_terms.Kd);
		c_Processor::host_serial.Write(13);
		
		
		if (return_value != NGC_Interpreter_Errors::OK)
		return return_value;
		
		//Configure drive output based on input values.
		
		//If we are speeding up set acceleration
		if (c_Parser::spindle_input.target_rpm > c_Spindle_Drive::Drive_Control.target_rpm)
		{
			c_Spindle_Drive::Set_State(STATE_BIT_ACCELERATE);
			c_Spindle_Drive::Clear_State(STATE_BIT_DECELERATE);
			c_Spindle_Drive::Drive_Control.encoder_at_state_change = c_Encoder_RPM::Encoder_Position();
		}
		//If we are slowing down set deceleration
		if (c_Parser::spindle_input.target_rpm < c_Spindle_Drive::Drive_Control.target_rpm)
		{
			c_Spindle_Drive::Clear_State(STATE_BIT_ACCELERATE);
			c_Spindle_Drive::Set_State(STATE_BIT_DECELERATE);
			c_Spindle_Drive::Drive_Control.encoder_at_state_change = c_Encoder_RPM::Encoder_Position();
		}
		c_Spindle_Drive::Drive_Control.target_rpm = c_Parser::spindle_input.target_rpm;
		
		c_Spindle_Drive::Drive_Control.motion_steps_per_encoder_tick =
		(c_Parser::spindle_input.feed_rate/(float)100.0)*160;
		
		c_Spindle_Drive::Drive_Control.feed_mode = c_Parser::spindle_input.feed_mode;
		
		c_Processor::host_serial.Write("accel");c_Processor::host_serial.print_float(c_Spindle_Drive::Drive_Control.accel_time);
		
		//c_Spindle_Drive::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
	}
	
	return return_value;
}

int16_t c_Parser::_ParseValues()
{
	/*
	Line is a char array of the original line that came in.
	A good portion of the time we will just be sending 'Line'
	as is without modifications. We need to save a copy of it
	after it comes out of the serial buffer, because the ring
	buffer in the serial class is a forward only buffer.
	If this value is used, it will be used in the PrepareLine
	method in the c_Machine class object.
	*/
	memset(c_Parser::Line,0,CYCLE_LINE_LENGTH);//<--Clear the line array
	char *line_pointer = c_Parser::Line;//<--creating a pointer to an array to simplify building the array data in the loops
	char thisValue = toupper(c_Processor::host_serial.Get());
	while (1)
	{
		
		//Is this a letter between A and Z
		if (thisValue >= 'A' && thisValue <= 'Z')
		{
			*line_pointer = thisValue;*line_pointer++;
			char Word = thisValue;
			char _address[10];
			memset(_address,0,10);
			uint8_t i=0;
			//move to first byte of address value
			thisValue = toupper(c_Processor::host_serial.Get());
			if (thisValue== 32) continue; //<-- Throw away spaces
			while(1)
			{
				if ((thisValue<'0' || thisValue>'9') && (thisValue!='.' && thisValue!='-'))
				{
					//We have a bad value. read through to the CR and toss out all the data. We are bailing!
					c_Processor::host_serial.Write("bad value <"); c_Processor::host_serial.Write_ni(thisValue); c_Processor::host_serial.Write(">\r");
					while(thisValue!=13)//<--loop until we get to CR so we don't process
					//the remainder of this line on return
					thisValue = c_Processor::host_serial.Get();

					return NGC_Interpreter_Errors::ADDRESS_VALUE_NOT_NUMERIC;
				}
				_address[i]=thisValue;i++;
				*line_pointer = thisValue;*line_pointer++;
				thisValue = toupper(c_Processor::host_serial.Get());
				if (thisValue== 32) //<-- throw away spaces
				thisValue = toupper(c_Processor::host_serial.Get());
				//If this is a letter, or a CR, break out of this inner while
				if ((thisValue >= 'A' && thisValue <= 'Z') || thisValue==13)
				break;
			}
			//host_serial.Write("<");
			//host_serial.Write(_address);
			//host_serial.Write(">");

			int ReturnValue = _groupWord(Word, _Evaluate_Address(_address));
			//If there was an error during line processing don't bother finishing the line, just bale.
			if (ReturnValue != NGC_Interpreter_Errors::OK)
			return ReturnValue;
		}
		//If this is a CR break out of the outer while
		if (thisValue==13)
		break;
	}
	*line_pointer = 13;*line_pointer++;
	return NGC_Interpreter_Errors::OK;
}

float c_Parser::_Evaluate_Address(char* Data)
{
	return atof(Data);
}

int16_t c_Parser::_groupWord(char Word, float Address)
{
	switch (Word)
	{
		case 'A': //internal PID factor tuning input
		{
			c_Spindle_Drive::Drive_Control.accel_time = Address;
		}
		
		case 'T': //internal PID factor tuning input
		{
			c_Spindle_Drive::Target_Position = Address;
			break;
		}
		case 'B': //internal PID factor tuning input
		{
			c_Spindle_Drive::B = Address;
			break;
		}
		case 'P': //internal PID factor tuning input
		{
			c_PID::spindle_terms.Kp = Address;
			break;
		}
		case 'I': //internal PID factor tuning input
		{
			c_PID::spindle_terms.Ki = Address;
			break;
		}
		case 'D': //internal PID factor tuning input
		{
			c_PID::spindle_terms.Kd = Address;
			break;
		}
		case 'S': //S is for speed of the spindle. This sets the target RPM
		{
			c_Parser::spindle_input.target_rpm=Address;
			break;
		}
		case 'F': //F is for distance per revolution.
		{
			c_Parser::spindle_input.feed_rate=Address;
			break;
		}
		case 'G': //G sets the feed rate mode. If the G address is not a valid feed rate mode, it is ignored.
		{
			switch ((uint16_t) Address) //Since this is only a spindle controller we do not
			//care about any gcodes not related to spindle functions
			{
				case 93:
				c_Parser::spindle_input.feed_mode=93;
				break;
				case 94:
				c_Parser::spindle_input.feed_mode=94;
				break;
				case 95:
				c_Parser::spindle_input.feed_mode=95;
				break;
			}
			c_Parser::spindle_input.feed_rate=Address;
			break;
		}
		case 'M': //<--Process words for M (M3,M90,M5, etc..)
		{
			switch ((int)Address)
			{
				
				case 3://<-- spindle on clockwise
				{
					c_Parser::spindle_input.rotation_direction=PARSER_DIRECTION_CW;
					//memset(Message,0,MESSAGE_LENGTH);
					//memcpy(Message,"Accelerating CW",17);
					//host_serial.Write("cw\r");
					break;
				}
				case 4://<-- spindle on counter-clockwise
				{
					c_Parser::spindle_input.rotation_direction=PARSER_DIRECTION_CCW;
					//memset(Message,0,MESSAGE_LENGTH);
					//memcpy(Message,"Accelerating C-CW",18);
					//host_serial.Write("c-cw\r");
					break;
				}
				case 5://<-- spindle stop
				{
					c_Parser::spindle_input.rotation_direction=PARSER_DIRECTION_OFF;
					//memset(Message,0,MESSAGE_LENGTH);
					//memcpy(Message,"Stopping",9);
					//host_serial.Write("stop\r");
					break;
				}
				case 19://<-- spindle orientation
				{
					c_Parser::spindle_input.roation_angle=360;
					//memset(Message,0,MESSAGE_LENGTH);
					//memcpy(Message,"Indexing",9);
					break;
				}
			}
			break;
		}
	}
	return NGC_Interpreter_Errors::OK;
}
