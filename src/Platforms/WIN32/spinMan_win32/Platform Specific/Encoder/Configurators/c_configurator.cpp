/* 
* c_configurator.cpp
*
* Created: 10/20/2019 11:27:15 AM
* Author: Family
*/


#include "c_configurator.h"
#include "../../../../../../driver/volatile_encoder_externs.h"
#include "../c_encoder_win.h"

void c_configurator::config_chz()
{
}

void c_configurator::config_cha()
{
}

void c_configurator::config_chb()
{
}

void c_configurator::configure_encoder_quadrature()
{
	//configure channel a on int0;
	c_configurator::config_cha();
	//configure channel b on int1;
	c_configurator::config_chb();
	
}

void c_configurator::configure_encoder_quadrature_w_z()
{
	//configure channels a/b on int0 and int1
	c_configurator::configure_encoder_quadrature();
	//configure the z index on pcint
	c_configurator::config_chz();	
}

void c_configurator::configure_test_channels()
{
	//Setup isr events for all 3 channels
	c_configurator::config_cha();
	c_configurator::config_chb();
	c_configurator::config_chz();

	//assign function pointer for testing to each of the isr callers

	//assign a function pointer to be called when a isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::test_channels;
	//assign a function pointer to be called when b isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::test_channels;
	//assign a function pointer to be called when z isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::test_channels;
}

void c_configurator::configure_disable_channels()
{
	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::no_vect;
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::no_vect;
	spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
}