/* 
* c_readers.cpp
*
* Created: 10/20/2019 1:09:44 PM
* Author: Family
*/


#include "c_readers.h"
#include "../../../../../../driver/volatile_encoder_externs.h"
#include "../c_encoder_avr_328.h"

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t enc_val = 0;

void c_readers::no_vect(){};

void c_readers::read_cha()
{
	//single channel can't determine direction so it only counts 'up'
	spindle_encoder.position ++;

	if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	spindle_encoder.position = 1;
}

void c_readers::read_chb()
{
	//single channel can't determine direction so it only counts 'up'
	spindle_encoder.position ++;

	if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	spindle_encoder.position = 1;
}

void c_readers::read_chz()
{

	//So long as the timer remains enabled we can track rpm.
	extern_encoder__ticks_at_time++;
	//reset the encoder position when z index is read
	spindle_encoder.position = 1;
}

void c_readers::read_quad()
{
	//UDR0='c';
	//uint32_t enc_ticks_per_rev = Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev;
	
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	spindle_encoder.direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	spindle_encoder.position += spindle_encoder.direction;
	
	if (spindle_encoder.position == 0)
	spindle_encoder.position = spindle_encoder.ticks_per_rev;
	else if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	spindle_encoder.position = 1;
	
	
	//So long as the timer remains enabled we can track rpm.
	extern_encoder__ticks_at_time++;
}

void c_readers::read_quad_with_z()
{
	//UDR0='c';
	//uint32_t enc_ticks_per_rev = Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev;
	
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	spindle_encoder.direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	spindle_encoder.position += spindle_encoder.direction;
	
	//We shouldn't need to track any of this if there is a z index

	//if (spindle_encoder.position == 0)
	//spindle_encoder.position = spindle_encoder.ticks_per_rev;
	//else if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	//spindle_encoder.position = 1;
	//
	//
	////So long as the timer remains enabled we can track rpm.
	//extern_encoder__ticks_at_time++;
}

uint8_t c_readers::read_active_channels()
{
	uint8_t _return = spindle_encoder.active_channels;
	return _return;
}
