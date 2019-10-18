#include "c_encoder_avr_328.h"
#include "c_inputs_avr_328.h"
#include "../../../../driver/c_input.h"
#include "../../../../bit_manipulation.h"
#include "../../../../driver/volatile_input_externs.h"
#define __ENCODER_VOLATILES__
#include "../../../../driver/volatile_encoder_externs.h"

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t enc_val = 0;

void HardwareAbstractionLayer::Encoder::no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
	spindle_encoder.position = 0;
	spindle_encoder.active_channels = 0;
	//spindle_encoder.ticks_per_rev = 400;
	/*
	These are default configs on start up. They may change entirely after
	a configuration is loaded in the c_configuration class.
	*/
	//setup isr's on int0, int1 for quadrature encoder with no index
	HardwareAbstractionLayer::Encoder::configure_encoder_quadrature();

	/*
	//assign a function pointer to be called when a isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_quad;
	//assign a function pointer to be called when b isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_quad;
	//assign a function pointer to be called when z isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
	//assign a function pointer to compute rpm based on encoder type.
	spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::get_rpm_quad;
	*/
	
}

uint8_t HardwareAbstractionLayer::Encoder::get_active_channels()
{
	uint8_t _return = spindle_encoder.active_channels;
	return _return;
}

void HardwareAbstractionLayer::Encoder::get_rpm()
{
	/*
	This is calling a function pointer to get rpm because depending on the type of encoder
	in use with the control, we can calculate rpm different ways. Without an indexed
	encoder, that method will probably be get_rpm_quad() because that is the default rpm
	method.
	*/
	spindle_encoder.func_vectors.Rpm_Compute();//<--check rpm, recalculate if its time
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
	//configure channel a on int0;
	HardwareAbstractionLayer::Encoder::config_cha();
	//configure channel b on int1;
	HardwareAbstractionLayer::Encoder::config_chb();
	
}

void HardwareAbstractionLayer::Encoder::configure_encoder_quadrature_w_z()
{
	//configure channels a/b on int0 and int1
	HardwareAbstractionLayer::Encoder::configure_encoder_quadrature();
	//configure the z index on pcint
	HardwareAbstractionLayer::Encoder::config_chz();
	
	
}

void HardwareAbstractionLayer::Encoder::configure_test_channels()
{
	//Setup isr events for all 3 channels
	HardwareAbstractionLayer::Encoder::config_cha();
	HardwareAbstractionLayer::Encoder::config_chb();
	HardwareAbstractionLayer::Encoder::config_chz();

	//assign function pointer for testing to each of the isr callers

	//assign a function pointer to be called when a isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::test_channels;
	//assign a function pointer to be called when b isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::test_channels;
	//assign a function pointer to be called when z isr runs.
	spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::test_channels;
}

void HardwareAbstractionLayer::Encoder::configure_disable_channels()
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

void HardwareAbstractionLayer::Encoder::get_rpm_quad()
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

void HardwareAbstractionLayer::Encoder::test_channels()
{
	//This method should only get called during auto configuration.
	//It will read port 'D' to determine which channels of the encoder
	//are active and then the auto config can decide which vectors to
	//point to.
	uint8_t capture_port = PIND;

	//Check pin 0. (int0)
	if (BitTst(capture_port,PIND0)) //<--encoder channel a
	spindle_encoder.active_channels |= ENC_CHA_TRK_BIT;

	//Check pin 1. (int1)
	if (BitTst(capture_port,PIND1)) //<--encoder channel b
	spindle_encoder.active_channels |= ENC_CHB_TRK_BIT;

	//Check pin 4. (pcint4)
	if (BitTst(capture_port,PIND4)) //<--encoder channel z
	spindle_encoder.active_channels |= ENC_CHZ_TRK_BIT;
}

void HardwareAbstractionLayer::Encoder::read_cha()
{
	//single channel can't determine direction so it only counts 'up'
	spindle_encoder.position ++;

	if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	spindle_encoder.position = 1;
}

void HardwareAbstractionLayer::Encoder::read_chb()
{
	//single channel can't determine direction so it only counts 'up'
	spindle_encoder.position ++;

	if (spindle_encoder.position >spindle_encoder.ticks_per_rev)
	spindle_encoder.position = 1;
}

void HardwareAbstractionLayer::Encoder::read_chz()
{
	UDR0='X';

	//So long as the timer remains enabled we can track rpm.
	extern_encoder__ticks_at_time++;
	//reset the encoder position when z index is read
	spindle_encoder.position = 1;
}

void HardwareAbstractionLayer::Encoder::read_quad()
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

void HardwareAbstractionLayer::Encoder::read_quad_with_z()
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


ISR (INT0_vect)
{
	//UDR0='a';
	spindle_encoder.func_vectors.Encoder_Vector_A(); //call a method dependent on what the configuration told us to call!
}

ISR(INT1_vect)
{
	//UDR0='b';
	spindle_encoder.func_vectors.Encoder_Vector_B(); //call a method dependent on what the configuration told us to call!
}

//ISR (PCINT1_vect)
//{
//uint8_t current = PIND ;
//
//
//Platform_Specific_HAL_Encoder_Vector_Z();
//
//if (current & INDEX_PIN)
//{
////extern_encoder__active_channels |= ENC_CHZ_TRK_BIT;
//}
//
//}
