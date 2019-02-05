/*
* c_Time_Keeper.cpp
*
* Created: 8/8/2018 3:30:41 PM
* Author: jeff_d
*/


#include "c_Time_Keeper.h"
#include "../Driver/c_Spindle_Drive.h"
#include "../FeedBack/c_Encoder_RPM.h"

volatile uint16_t time_ticks = 0;
volatile uint32_t _millis = 0;

void c_Time_Keeper::Initialize()
{
	/*
	CS02	CS01	CS00	DESCRIPTION
	0		0 		0		Timer/Counter2 Disabled
	0		0 		1 		No Prescaling
	0		1 		0 		Clock / 8
	0		1 		1 		Clock / 64
	1		0 		0 		Clock / 256
	1		0 		1 		Clock / 1024
	*/

	//TIMSK0 |=(1<<TOIE0);
	//TIMSK0 |= (1 << OCIE0B);
	
	//TCCR0B |= (1<<CS02) | (1<<CS00);// | _BV(WGM02);
	//TCCR0A |= (1<<WGM01) | (1<<WGM00);
	//OCR0B=1;
	
	TCNT2   = 0;
	TCCR2A |= (1 << WGM21); // Configure timer 2 for CTC mode
	TCCR2B |= (1 << CS22); // Start timer at Fcpu/64
	TIMSK2 |= (1 << OCIE2A); // Enable CTC interrupt
	OCR2A   = 250; // Set CTC compare value with a prescaler of 64
}

uint32_t c_Time_Keeper::millis()
{
	return _millis;
}

ISR(TIMER2_COMPA_vect)
{
	
	//each time_tick is .0001 seconds with prescaler of 8
	time_ticks++;
	_millis++;
	if (time_ticks>=10)
	{
		time_ticks -= 10;
		//c_Encoder_RPM::SetAverageRPM();
	}
		
}

//// default constructor
//c_Time_Keeper::c_Time_Keeper()
//{
//} //c_Time_Keeper
//
//// default destructor
//c_Time_Keeper::~c_Time_Keeper()
//{
//} //~c_Time_Keeper
