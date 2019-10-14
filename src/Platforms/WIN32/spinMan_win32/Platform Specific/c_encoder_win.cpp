#include "c_encoder_win.h"
#include "c_inputs_win.h"
//#include "..\..\..\..\c_configuration.h"

#define __ENCODER_VOLATILES__
#include "volatile_encoder_externs.h"

static const int8_t encoder_table[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

void HardwareAbstractionLayer::Encoder::no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
}

uint32_t HardwareAbstractionLayer::Encoder::get_position()
{
	return 0;
}

void HardwareAbstractionLayer::Encoder::config_chz()
{
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
uint8_t HardwareAbstractionLayer::Encoder::get_active_channels()
{
	uint8_t _return = extern_encoder__active_channels;
	return _return;
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
