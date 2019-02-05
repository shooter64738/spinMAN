/*
* c_Spindle.cpp
*
* Created: 3/6/2018 1:21:49 PM
* Author: jeff_d
*/


#include "c_Spindle.h"

// default constructor
//c_Spindle::c_Spindle()
//{
//} //c_Spindle
//
//c_Spindle::c_Spindle(c_Serial host_serial)
//{
//this->host_serial = host_serial;
//memset(Message,0,MESSAGE_LENGTH);
//memcpy(Message,"Idle",5);
//}

//// default destructor
//c_Spindle::~c_Spindle()
//{
//} //~c_Spindle
//
//void c_Spindle::ProcessLine()
//{
//_ParseValues();
//}

int c_Spindle::_ParseValues()
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
	memset(Line,0,CYCLE_LINE_LENGTH);//<--Clear the line array
	char *line_pointer = Line;//<--creating a pointer to an array to simplify building the array data in the loops
	char thisValue = toupper(host_serial.Get());
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
			thisValue = toupper(host_serial.Get());
			if (thisValue== 32) continue; //<-- Throw away spaces
			while(1)
			{
				if ((thisValue<'0' || thisValue>'9') && (thisValue!='.' && thisValue!='-'))
				{
					//We have a bad value. read through to the CR and toss out all the data. We are bailing!
					host_serial.Write("bad value <"); host_serial.Write_ni(thisValue); host_serial.Write(">\r");
					while(thisValue!=13)//<--loop until we get to CR so we don't process
					//the remainder of this line on return
					thisValue = host_serial.Get();

					return NGC_Interpreter_Errors::ADDRESS_VALUE_NOT_NUMERIC;
				}
				_address[i]=thisValue;i++;
				*line_pointer = thisValue;*line_pointer++;
				thisValue = toupper(host_serial.Get());
				if (thisValue== 32) //<-- throw away spaces
				thisValue = toupper(host_serial.Get());
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

float c_Spindle::_Evaluate_Address(char* Data)
{
	return atof(Data);
}

int c_Spindle::_groupWord(char Word, float Address)
{
	switch (Word)
	{
		case 'S': //S is for speed of the spindle. This sets the target RPM
		{
			c_Spindle::spindle_input.target_rpm=Address;
			break;
		}
		case 'F': //S is for speed of the spindle. This sets the target RPM
		{
			c_Spindle::spindle_input.target_rpm=Address;
			break;
		}
		case 'M': //<--Process words for M (M3,M90,M5, etc..)
		{
			switch ((int)Address)
			{
				
				case 3://<-- spindle on clockwise
				{
					c_Spindle::spindle_input.rotation_direction=DIRECTION_CW;
					memset(Message,0,MESSAGE_LENGTH);
					memcpy(Message,"Accelerating CW",17);
					//host_serial.Write("cw\r");
					break;
				}
				case 4://<-- spindle on counter-clockwise
				{
					c_Spindle::spindle_input.rotation_direction=DIRECTION_CCW;
					memset(Message,0,MESSAGE_LENGTH);
					memcpy(Message,"Accelerating C-CW",18);
					//host_serial.Write("c-cw\r");
					break;
				}
				case 5://<-- spindle stop
				{
					c_Spindle::spindle_input.rotation_direction=DIRECTION_OFF;
					memset(Message,0,MESSAGE_LENGTH);
					memcpy(Message,"Stopping",9);
					//host_serial.Write("stop\r");
					break;
				}
				case 19://<-- spindle orientation
				{
					c_Spindle::spindle_input.roation_angle=0;
					memset(Message,0,MESSAGE_LENGTH);
					memcpy(Message,"Indexing",9);
					break;
				}
			}
			return 0;
			break;
		}
		default:
		return 0;
	}
	return NGC_Interpreter_Errors::OK;
}
