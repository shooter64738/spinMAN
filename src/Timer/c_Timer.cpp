/*
* c_Timer.cpp
*
* Created: 3/5/2018 9:49:35 PM
* Author: jeff_d
*/


#include "c_Timer.h"

// default constructor
c_Timer::c_Timer()
{
} //c_Timer

// default destructor
c_Timer::~c_Timer()
{
} //~c_Timer

void c_Timer::Initialize()
{
	TCCR1B |= (1 << WGM12); // configure timer1 for CTC mode
	TIMSK1 |= (1 << OCIE1A); // enable the CTC interrupt
	OCR1A   = 3906; //3906;//<--every 250 milli seconds; //7812; //<-- every 500 milli seconds; 15625;//<-- every 1000 mili seconds
	TCCR1B |= ((1 << CS10) | (1 << CS12)); // start the timer at 16Mhz/1024
	
	millis=0;
}
unsigned long c_Timer::Millis()
{
return millis;
}

void c_Timer::Trigger()
{
	millis++;
}

ISR(TIMER1_COMPA_vect)
{ // this function is called every time the timer reaches the threshold we set

	c_Timer::Trigger();

}