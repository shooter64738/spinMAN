/*
* c_Processor.cpp
*
* Created: 8/18/2018 12:17:34 PM
* Author: Family
*/


#include "c_Processor.h"
#include "FeedBack\c_Encoder_RPM.h"
#include "Driver\c_Spindle_Drive.h"
#include "Time\c_Time_Keeper.h"
#include "Parser\c_Parser.h"
#include "PID\c_PID.h"
#include "globals.h"

c_Serial c_Processor::host_serial;

void c_Processor::Start()
{
	c_Processor::host_serial = c_Serial(0,115200);//<-- Start serial at 115,200 baud on port 0
	c_Processor::host_serial.print_string("spinMAN v0.1");//<-- Send hello message
	
	c_Encoder_RPM::Initialize(100); //<--init encoder, specify ticks per rev for encoder
	c_Spindle_Drive::Initialize(); //<--init pwm spindle output
	c_Time_Keeper::Initialize(); //<--init time keeper.. for time, obviously
	c_Parser::Initialize(); //<--init the gcode parser for spindle commands
	c_PID::Initialize();//<--init the pid for spindle and servo control
	
	uint16_t ticker =0;
	sei(); //<-- enable global interrupts

	
	//LED on pin 13 of UNO. For testing - jumper pin 6 (pwm output) to pin 13 on the uno. LED should change brightness depending on encoder inputs.
	DDRB &= ~(1 << PB5);
	//PORTB |= (1 << PINB5); //enable pullup
	int16_t return_value = NGC_Interpreter_Errors::OK;
	while (1)
	{
		ticker++;
		
		return_value = c_Parser::CheckInput();//<--check for serial input from the host
		if (return_value !=NGC_Interpreter_Errors::OK)
		{
			c_Processor::host_serial.print_string("e-stop. data error.");
			c_Spindle_Drive::Disable_Drive();
			//Error reading spindle control data. This would be an estop condition.
			while(1)
			{
				//stuck in a loop. reset required
			}
		}
		
		//Any data that was set from the gcode parsing will be acted on here
		return_value = c_Spindle_Drive::Check_State();
		
		
		if (return_value>0)
		{
			c_Processor::host_serial.print_string("e-stop. illegal rotation detected.");
			c_Spindle_Drive::Disable_Drive();
			//Error reading spindle control data. This would be an estop condition.
			//while(1)
			{
				//stuck in a loop. reset required
			}
		}
		ticker++;
		if (ticker > 20000)
		{
			ticker=0;
			//c_Processor::host_serial.Write("i-rpm = ");
			//c_Processor::host_serial.print_int32(c_Encoder_RPM::CurrentRPM());
			//c_Processor::host_serial.Write("a-rpm = ");
			c_Processor::host_serial.print_int32(c_Encoder_RPM::CurrentRPM());
			//uint32_t avg=0;
			//for (int i=0;i<TIME_ARRAY_SIZE;i++)
			//{
				//avg+=time_at_tick[i];
				//c_Processor::host_serial.print_int32(time_at_tick[i]);
				//c_Processor::host_serial.print_string(",");
			//}
			//float seconds = ((((float)avg)/((float)TIME_ARRAY_SIZE))*TIME_FACTOR)*100.0;
			//c_Processor::host_serial.print_float(60.0/seconds);
			c_Processor::host_serial.Write(CR);
			//c_Processor::host_serial.Write("  pos = ");
			//c_Processor::host_serial.print_int32(c_Encoder_RPM::Encoder_Position());
		}
		
		
		
		////c_Spindle_Drive::current_output = 178;
		////OCR0A = c_Spindle_Drive::current_output;
		//if (c_Encoder_RPM::AverageRPM() < Setpoint)
		//{
		////c_Spindle_Drive::Accelerate();
		//}
		//else if (c_Encoder_RPM::AverageRPM() > Setpoint)
		//{
		////c_Spindle_Drive::Decelerate();
		//}
		
		//ticks++;
		//if (ticks>6000)
		//{
		//host_serial.Write("inst rpm:");host_serial.print_int32(c_Encoder_RPM::AverageRPM());
		//host_serial.Write("out put:");host_serial.print_int32(c_Spindle_Drive::current_output);
		//ticks=0;
		//}
	}
}



//// default constructor
//c_Processor::c_Processor()
//{
//} //c_Processor
//
//// default destructor
//c_Processor::~c_Processor()
//{
//} //~c_Processor
