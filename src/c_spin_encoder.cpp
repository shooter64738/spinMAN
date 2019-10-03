/* 
* c_spin_encoder.cpp
*
* Created: 10/1/2019 7:58:54 PM
* Author: Family
*/


#include "c_spin_encoder.h"

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
volatile uint32_t enc_count = 0;	
static uint8_t enc_val = 0;

void Spin::Input::Encoder::initialize()
{
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1
	
	//EICRA |= (1 << ISC00) | (1 << ISC01);	// Trigger on rising change on INT0
	//EICRA |= (1 << ISC10) | (1 << ISC11);	// Trigger on rising change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on falling change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on falling change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on hi change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on hi change on INT1
	
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
	
}

void Spin::Input::Encoder::encoder_update()
{
	//////////////////////////////////////////////////////////////////////////
	 // static allows this value to persist across function calls
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	int8_t encoder_direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	enc_count += encoder_direction;
	
	if (enc_count<= 0)
	enc_count = encoder_ticks_per_rev;
	else if (enc_count >=encoder_ticks_per_rev)
	enc_count = 0;
	//////////////////////////////////////////////////////////////////////////
}

ISR (INT0_vect)
{
	UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	Spin::Input::Encoder::encoder_update();
}

ISR(INT1_vect)
{
	UDR0='b';
	Spin::Input::Encoder::encoder_update();
}