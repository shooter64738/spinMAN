/* 
* c_readers.cpp
*
* Created: 10/20/2019 1:09:44 PM
* Author: Family
*/


#include "c_readers.h"
#include "../../../../../../driver/volatile_encoder_externs.h"
#include "../c_encoder_win.h"

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
}

uint8_t c_readers::read_active_channels()
{
	uint8_t _return = spindle_encoder.active_channels;
	return _return;
}
