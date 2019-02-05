/* 
* c_Timer.h
*
* Created: 3/5/2018 9:49:35 PM
* Author: jeff_d
*/


#ifndef __C_TIMER_H__
#define __C_TIMER_H__
#include <avr/io.h>
#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>    // Needed to use interrupts

volatile static unsigned long millis;
class c_Timer
{
//variables
public:
protected:
private:

//functions
public:
	c_Timer();
	~c_Timer();
	
	static void Initialize();
	static void Trigger();
	static unsigned long Millis();
protected:
private:
	c_Timer( const c_Timer &c );
	c_Timer& operator=( const c_Timer &c );

}; //c_Timer

#endif //__C_TIMER_H__
