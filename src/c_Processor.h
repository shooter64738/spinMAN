
/*
* c_Processor.h
*
* Created: 8/18/2018 12:17:34 PM
* Author: Family
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial\c_Serial.h"



class c_Processor
{
	//variables
	public:
	static c_Serial host_serial;
	

	protected:
	private:

	//functions
	public:
	static void Start();
	

	protected:
	private:
	//c_Processor();
	//~c_Processor();
	//c_Processor( const c_Processor &c );
	//c_Processor& operator=( const c_Processor &c );

}; //c_Processor

#endif //__C_PROCESSOR_H__
