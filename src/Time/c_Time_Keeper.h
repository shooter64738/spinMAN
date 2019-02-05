/* 
* c_Time_Keeper.h
*
* Created: 8/8/2018 3:30:41 PM
* Author: jeff_d
*/


#ifndef __C_TIME_KEEPER_H__
#define __C_TIME_KEEPER_H__
#include <avr/io.h>
#include <avr/interrupt.h>

class c_Time_Keeper
{
//variables
public:
protected:
private:

//functions
public:
	static void Initialize();
	static uint32_t millis();

protected:
private:
	c_Time_Keeper( const c_Time_Keeper &c );
	c_Time_Keeper& operator=( const c_Time_Keeper &c );
	c_Time_Keeper();
	~c_Time_Keeper();

}; //c_Time_Keeper

#endif //__C_TIME_KEEPER_H__
