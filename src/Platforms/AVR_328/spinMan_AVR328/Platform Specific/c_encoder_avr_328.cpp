#include "c_encoder_avr_328.h"
#include "c_inputs_avr_328.h"
//#include "..\..\..\..\c_configuration.h"

#define __ENCODER_VOLATILES__
#include "volatile_encoder_externs.h"

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
//static const int8_t encoder_table[] = {10,11,22,33,4,5,6,7,8,9,10,11,12,13,14,15};
static uint8_t enc_val = 0;

void HardwareAbstractionLayer::Encoder::no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
	extern_encoder__count = 0;
	extern_encoder__active_channels = 0;
	extern_encoder__ticks_per_rev = 400;
	Platform_Specific_HAL_Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_quad; // HardwareAbstractionLayer::Encoder::no_vect;
	Platform_Specific_HAL_Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_quad; //HardwareAbstractionLayer::Encoder::no_vect;
	Platform_Specific_HAL_Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
	HardwareAbstractionLayer::Encoder::config_cha();
	HardwareAbstractionLayer::Encoder::config_chb();
}

uint32_t HardwareAbstractionLayer::Encoder::get_position()
{
	uint32_t _enc_count = extern_encoder__count;
	return _enc_count;
}

uint8_t HardwareAbstractionLayer::Encoder::get_active_channels()
{
	uint8_t _return = extern_encoder__active_channels;
	return _return;
}

void HardwareAbstractionLayer::Encoder::config_chz()
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

void HardwareAbstractionLayer::Encoder::config_cha()
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

void HardwareAbstractionLayer::Encoder::config_chb()
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

void HardwareAbstractionLayer::Encoder::configure_encoder_quadrature()
{
	HardwareAbstractionLayer::Encoder::config_cha();
	HardwareAbstractionLayer::Encoder::config_chb();
	HardwareAbstractionLayer::Encoder::config_chz();
	
	////Set the mask to check pins PB0-PB5
	//PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3);
	////Set the interrupt for PORTB (PCIE0)
	//PCICR |= (1<<PCIE0);
	//PCIFR |= (1<<PCIF0);
	/*
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
	*/
}

void HardwareAbstractionLayer::Encoder::read_cha()
{
}

void HardwareAbstractionLayer::Encoder::read_chb()
{
}

void HardwareAbstractionLayer::Encoder::read_chz()
{
}

void HardwareAbstractionLayer::Encoder::read_quad()
{
	//UDR0='c';
	//uint32_t enc_ticks_per_rev = Spin::Configuration::Drive_Settings.Encoder_Config.Encoder_Ticks_Per_Rev;
	
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	extern_encoder__direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	extern_encoder__count += extern_encoder__direction;
	
	if (extern_encoder__count == 0)
	extern_encoder__count = extern_encoder__ticks_per_rev;
	else if (extern_encoder__count >extern_encoder__ticks_per_rev)
	extern_encoder__count = 1;
	
	
	//So long as the timer remains enabled we can track rpm.
	enc_ticks_at_current_time++;
}


ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	extern_encoder__active_channels |= ENC_CHA_TRK_BIT;
	Platform_Specific_HAL_Encoder_Vector_A(); //call a method dependent on what the configuration told us to call!
	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
	
}

ISR(INT1_vect)
{
	//UDR0='b';
	extern_encoder__active_channels |= ENC_CHB_TRK_BIT;
	Platform_Specific_HAL_Encoder_Vector_B(); //call a method dependent on what the configuration told us to call!
	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
}

ISR (PCINT1_vect)
{
	uint8_t current = PIND ;
	
	
	Platform_Specific_HAL_Encoder_Vector_Z();
	
	if (current & INDEX_PIN)
	{
		//extern_encoder__active_channels |= ENC_CHZ_TRK_BIT;
	}
	
}
