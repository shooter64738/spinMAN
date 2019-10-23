#include "c_encoder_sam_d21.h"
#include "../c_inputs_sam_d21.h"
#include "../../../../../driver/c_input.h"
#include "../../../../../bit_manipulation.h"
#include "../../../../../driver/volatile_input_externs.h"
#define __ENCODER_VOLATILES__
#include "../../../../../driver/volatile_encoder_externs.h"


static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t enc_val = 0;


void HardwareAbstractionLayer::Encoder::initialize()
{
	spindle_encoder.position = 1;
	spindle_encoder.active_channels = 0;
	//spindle_encoder.ticks_per_rev = 400;
	/*
	These are default configs on start up. They may change entirely after
	a configuration is loaded in the c_configuration class.
	*/
	//setup isr's on int0, int1 for quadrature encoder with no index
	//HardwareAbstractionLayer::Encoder::configure_encoder_quadrature();

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

void HardwareAbstractionLayer::Encoder::test_channels()
{
	//This method should only get called during auto configuration.
	//It will read port 'D' to determine which channels of the encoder
	//are active and then the auto config can decide which vectors to
	//point to.
	//uint8_t capture_port = PIND;

	//Check pin 0. (int0)
	//if (BitTst(capture_port,PIND0)) //<--encoder channel a
	//spindle_encoder.active_channels |= ENC_CHA_TRK_BIT;

	//Check pin 1. (int1)
	//if (BitTst(capture_port,PIND1)) //<--encoder channel b
	//spindle_encoder.active_channels |= ENC_CHB_TRK_BIT;

	//Check pin 4. (pcint4)
	//if (BitTst(capture_port,PIND4)) //<--encoder channel z
	//spindle_encoder.active_channels |= ENC_CHZ_TRK_BIT;
}

//ISR (INT0_vect)
//{
	////UDR0='a';
	//spindle_encoder.func_vectors.Encoder_Vector_A(); //call a method dependent on what the configuration told us to call!
//}
//
//ISR(INT1_vect)
//{
	////UDR0='b';
	//spindle_encoder.func_vectors.Encoder_Vector_B(); //call a method dependent on what the configuration told us to call!
//}

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

