#include "c_encoder_win.h"
#include "c_inputs_win.h"
//#include "..\..\..\..\c_configuration.h"

#define __ENCODER_VOLATILES__
#include "volatile_encoder_externs.h"

static const int8_t encoder_table[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

void HardwareAbstractionLayer::Encoder::no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
	extern_encoder__enc_count = 1;
	extern_encoder__active_channels = 0;
	Platform_Specific_HAL_Encoder_Vector_A = HardwareAbstractionLayer::Encoder::no_vect;
	Platform_Specific_HAL_Encoder_Vector_B = HardwareAbstractionLayer::Encoder::no_vect;
	Platform_Specific_HAL_Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
}

uint32_t HardwareAbstractionLayer::Encoder::get_position()
{
	uint32_t _enc_count = extern_encoder__enc_count;
	return _enc_count;
}

uint8_t HardwareAbstractionLayer::Encoder::get_encoder_active()
{
	return extern_encoder__active_channels;
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
	
}

void HardwareAbstractionLayer::Encoder::config_chb()
{
}

void HardwareAbstractionLayer::Encoder::configure_encoder_quadrature()
{
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
}


//ISR(INT0_vect)
//{
//	//UDR0='a';
//	//c_Encoder_RPM::Encoder_Trigger();
//	extern_encoder__active_channels |= ENC_CHA_TRK_BIT;
//	Platform_Specific_HAL_Encoder_Vector_A(); //call a method dependent on what the configuration told us to call!
//	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
//
//}
//
//ISR(INT1_vect)
//{
//	//	UDR0='b';
//	extern_encoder__active_channels |= ENC_CHB_TRK_BIT;
//	Platform_Specific_HAL_Encoder_Vector_B(); //call a method dependent on what the configuration told us to call!
//	//HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
//}
//
//ISR(PCINT1_vect)
//{
//	uint8_t current = PIND;
//
//
//	Platform_Specific_HAL_Encoder_Vector_Z();
//
//	if (current & INDEX_PIN)
//	{
//		extern_encoder__active_channels |= ENC_CHZ_TRK_BIT;
//	}
//
//}
//
