/* 
* c_calculators.cpp
*
* Created: 10/20/2019 1:23:49 PM
* Author: Family
*/


#include "c_calculators.h"
#include "../../../../../../driver/volatile_encoder_externs.h"
#include "../../../../../../driver/volatile_input_externs.h"
#include "../../../../../../driver/c_input.h"
#include "../../../../../../bit_manipulation.h"

void c_calculators::no_vect(){};

void c_calculators::get_rpm()
{
	/*
	This is calling a function pointer to get rpm because depending on the type of encoder
	in use with the control, we can calculate rpm different ways. Without an indexed
	encoder, that method will probably be get_rpm_quad() because that is the default rpm
	method.
	*/
	if (spindle_encoder.func_vectors.Rpm_Compute != NULL)
	spindle_encoder.func_vectors.Rpm_Compute();//<--check rpm, recalculate if its time
}

void c_calculators::get_rpm_index()
{
	if (!BitTst(extern_input__intervals,RPM_INTERVAL_BIT))
	return;//<--return if its not time
	
	BitClr_(extern_input__intervals, RPM_INTERVAL_BIT);
	
	//doing some scaling up and down trying to avoid float math as much as possible.
	int32_t rps = (((float)spindle_encoder.period_ticks * RPM_PERIODS_IN_INTERVAL)) * 60.0;
	
	
	spindle_encoder.sensed_rpm = rps;
}

void c_calculators::get_rpm_quad()
{
	if (!BitTst(extern_input__intervals,RPM_INTERVAL_BIT))
	return;//<--return if its not time
	
	//In velocity mode we only care if the sensed rpm matches the input rpm
	//In position mode we only care if the sensed position matches the input position.

	BitClr_(extern_input__intervals, RPM_INTERVAL_BIT);
	
	//doing some scaling up and down trying to avoid float math as much as possible.
	int32_t rps = (((float)spindle_encoder.period_ticks * RPM_PERIODS_IN_INTERVAL) /(float)spindle_encoder.ticks_per_rev) * 60.0;
	
	
	spindle_encoder.sensed_rpm = rps;
}

void c_calculators::get_rpm_single_ch()
{
	if (!BitTst(extern_input__intervals,RPM_INTERVAL_BIT))
	return;//<--return if its not time
	
	//In velocity mode we only care if the sensed rpm matches the input rpm
	//In position mode we only care if the sensed position matches the input position.

	BitClr_(extern_input__intervals, RPM_INTERVAL_BIT);
	
	//doing some scaling up and down trying to avoid float math as much as possible.
	int32_t rps = (((float)spindle_encoder.period_ticks * RPM_PERIODS_IN_INTERVAL) /(float)spindle_encoder.ticks_per_rev) * 60.0;
	
	
	spindle_encoder.sensed_rpm = rps;
}