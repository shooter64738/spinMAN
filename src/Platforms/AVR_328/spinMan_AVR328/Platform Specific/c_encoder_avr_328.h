#ifndef __C_ENCODER_328_P_H__
#define __C_ENCODER_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

#define ENCODER_SUM_ARRAY_SIZE 8
#define ENCODER_SUM_SIZE_MSK (ENCODER_SUM_ARRAY_SIZE-1U)

#define ENC_CHA_TRK_BIT (1<<0)
#define ENC_CHB_TRK_BIT (1<<1)
#define ENC_CHZ_TRK_BIT (1<<2)



namespace HardwareAbstractionLayer
{
	class Encoder
	{
		public:
		
		
		static void initialize();
		static uint32_t get_position();
		static void configure_encoder_z_index();
		static void configure_encoder_simple_int_a();
		static void configure_encoder_simple_int_b();
		static void configure_encoder_quadrature();
		static void update_encoder_for_quad();
		static uint8_t get_encoder_active();
	};
};

#endif

