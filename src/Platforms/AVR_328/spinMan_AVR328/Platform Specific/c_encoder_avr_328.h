#ifndef __C_ENCODER_328_P_H__
#define __C_ENCODER_328_P_H__

#include <avr/io.h>
#include <avr/interrupt.h>

//#define ENCODER_SUM_ARRAY_SIZE 8
//#define ENCODER_SUM_SIZE_MSK (ENCODER_SUM_ARRAY_SIZE-1U)

#define ENC_CHA_TRK_BIT (1<<0)
#define ENC_CHB_TRK_BIT (1<<1)
#define ENC_CHZ_TRK_BIT (1<<2)

#define INDEX_PIN PIND4	//Nano pin D4

namespace HardwareAbstractionLayer
{
	class Encoder
	{
		public:
		
		
		static void no_vect();
		static void initialize();
		static uint32_t get_position();
		static void config_chz();
		static void config_cha();
		static void config_chb();
		static void configure_encoder_quadrature();
		static void get_rpm();
		static void get_rpm_quad();
		static void read_cha();
		static void read_chb();
		static void read_chz();
		static void read_quad();
		static uint8_t get_active_channels();
	};
};

#endif

