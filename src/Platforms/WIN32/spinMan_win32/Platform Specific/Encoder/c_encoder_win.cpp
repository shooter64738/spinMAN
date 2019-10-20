#include "c_encoder_win.h"
#include "../c_inputs_win.h"
#include "../../../../../driver/c_input.h"
#include "../../../../../bit_manipulation.h"
#include "../../../../../driver/volatile_input_externs.h"
#define __ENCODER_VOLATILES__
#include "../../../../../driver/volatile_encoder_externs.h"

static const int8_t encoder_table[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };


void HardwareAbstractionLayer::Encoder::no_vect(){};

void HardwareAbstractionLayer::Encoder::initialize()
{
	spindle_encoder.position = 0;
	spindle_encoder.active_channels = 0;
	//spindle_encoder.ticks_per_rev = 400;
	//spindle_encoder.func_vectors.Encoder_Vector_A = HardwareAbstractionLayer::Encoder::read_quad; // HardwareAbstractionLayer::Encoder::no_vect;
	//spindle_encoder.func_vectors.Encoder_Vector_B = HardwareAbstractionLayer::Encoder::read_quad; //HardwareAbstractionLayer::Encoder::no_vect;
	//spindle_encoder.func_vectors.Encoder_Vector_Z = HardwareAbstractionLayer::Encoder::no_vect;
	//spindle_encoder.func_vectors.Rpm_Compute = HardwareAbstractionLayer::Encoder::get_rpm_quad;
	//HardwareAbstractionLayer::Encoder::config_cha();
	//HardwareAbstractionLayer::Encoder::config_chb();
}

void HardwareAbstractionLayer::Encoder::test_channels()
{
	//This method should only get called during auto configuration.
	//It will read port 'D' to determine which channels of the encoder
	//are active and then the auto config can decide which vectors to
	//point to.
	uint8_t capture_port = 0;

	//Check pin 0. (int0)
	if (BitTst(capture_port, 0)) //<--encoder channel a
		spindle_encoder.active_channels |= ENC_CHA_TRK_BIT;

	//Check pin 1. (int1)
	if (BitTst(capture_port, 1)) //<--encoder channel b
		spindle_encoder.active_channels |= ENC_CHB_TRK_BIT;

	//Check pin 4. (pcint4)
	if (BitTst(capture_port, 2)) //<--encoder channel z
		spindle_encoder.active_channels |= ENC_CHZ_TRK_BIT;
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
