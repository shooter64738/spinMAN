#include "c_encoder_avr_328.h"
#include "c_inputs_avr_328.h"
//#include "..\..\..\..\c_configuration.h"

#define __ENCODER_VOLATILES__
#include "volatile_encoder_externs.h"

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

void no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
	enc_count = 1;
	enc_active_channels = 0;
	Platform_Specific_HAL_Encoder_Vector = no_vect;
}

uint32_t HardwareAbstractionLayer::Encoder::get_position()
{
	uint32_t _enc_count = enc_count;
	return _enc_count;
}

uint8_t get_encoder_active()
{
	return enc_active_channels;
}

void HardwareAbstractionLayer::Encoder::configure_encoder_z_index()
{
	//Pin D4 (PIND4) is used to read the index pulse from an encoder
	//set pin to input
	STEP_PORT_DIRECTION &= ~((1 << DDD4));
	//enable pull up
	STEP_PORT |= (1<<PORTD4);
}

void HardwareAbstractionLayer::Encoder::configure_encoder_simple_int_a()
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
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void HardwareAbstractionLayer::Encoder::configure_encoder_simple_int_b()
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
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void HardwareAbstractionLayer::Encoder::configure_encoder_quadrature()
{
	////Set the mask to check pins PB0-PB5
	//PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3);
	////Set the interrupt for PORTB (PCIE0)
	//PCICR |= (1<<PCIE0);
	//PCIFR |= (1<<PCIF0);

	//Setup encoder capture
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	EICRA |= (1 << ISC00);	// Trigger on any change on INT0 PD2 (pin D2)
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1 PD3 (pin D3)
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void HardwareAbstractionLayer::Encoder::update_encoder_for_quad()
{
	//uint32_t enc_ticks_per_rev = Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev;
	uint8_t enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	int8_t encoder_direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	enc_count += encoder_direction;
	
	if (enc_count == 0)
	enc_count = enc_ticks_per_rev;
	else if (enc_count >enc_ticks_per_rev)
	enc_count = 1;
	
	//So long as the timer remains enabled we can track rpm.
	enc_ticks_at_current_time++;
}


ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	enc_active_channels |= ENC_CHA_TRK_BIT;
	Platform_Specific_HAL_Encoder_Vector(); //call a method dependent on what the configuration told us to call!
	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
	
}

ISR(INT1_vect)
{
	//	UDR0='b';
	enc_active_channels |= ENC_CHB_TRK_BIT;
	Platform_Specific_HAL_Encoder_Vector(); //call a method dependent on what the configuration told us to call!
	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
}

