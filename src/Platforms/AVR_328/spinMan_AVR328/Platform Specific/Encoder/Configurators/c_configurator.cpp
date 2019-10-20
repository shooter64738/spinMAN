/* 
* c_configurator.cpp
*
* Created: 10/20/2019 11:27:15 AM
* Author: Family
*/


#include "c_configurator.h"
#include "../../../../../../driver/volatile_encoder_externs.h"
#include "../c_encoder_avr_328.h"

void c_configurator::config_chz()
{
	//DDRD &= ~(1 << DDD4);	//input mode
	//PORTD |= (1 << PORTD4);	//enable pullup
	//
	////Set the mask to check pin PD4
	//PCMSK2 = (1<<PCINT19);
	//
	////Set the interrupt for PORTD (PCIE1)
	//PCICR |= (1<<PCIE1);
	//PCIFR |= (1<<PCIF1);
}

void c_configurator::config_cha()
{
	////Set the mask to check pins PB0-PB5
	//PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3);
	////Set the interrupt for PORTB (PCIE0)
	//PCICR |= (1<<PCIE0);
	//PCIFR |= (1<<PCIF0);

	//Setup encoder capture
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	//DDRD &= ~(1 << DDD3);	//input mode
	//PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	EICRA |= (1 << ISC00);	// Trigger on any change on INT0 PD2 (pin D2)
	//EICRA |= (1 << ISC10);	// Trigger on any change on INT1 PD3 (pin D3)
	EIMSK |= (1 << INT0);// | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void c_configurator::config_chb()
{
	////Set the mask to check pins PB0-PB5
	//PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3);
	////Set the interrupt for PORTB (PCIE0)
	//PCICR |= (1<<PCIE0);
	//PCIFR |= (1<<PCIF0);

	//Setup encoder capture
	//DDRD &= ~(1 << DDD2);	//input mode
	//PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	//EICRA |= (1 << ISC00);	// Trigger on any change on INT0 PD2 (pin D2)
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1 PD3 (pin D3)
	EIMSK |= (1 << INT1);// | (1 << INT1);     // Enable external interrupt INT0, INT1
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
	//disable the hardware reading channel a. this will disable the isr
	EICRA &= ~(1 << ISC00);	// Trigger on any change on INT0 PD2 (pin D2)
	EIMSK &= ~(1 << INT0);// disable external interrupt INT0
	//disable the hardware reading channel b. this will disable the isr
	EICRA &= ~(1 << ISC10);	// Trigger on any change on INT1 PD3 (pin D3)
	EIMSK &= ~(1 << INT1);// disable external interrupt INT1
	//disable the hardware reading channel z. this will disable the isr
	//PCMSK2 &= ~(1<<PCINT19); // disable pin change interrupt pcint19

	//assign function pointers that does nothing. with the isrs disabled
	//this should have no effect at all.

	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::no_vect;
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::no_vect;
	spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
}