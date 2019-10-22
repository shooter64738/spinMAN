#ifndef __C_ENCODER_SAM_D21_H__
#define __C_ENCODER_SAM_D21_H__

#include "Configurators/c_configurator.h"
#include "Readers/c_readers.h"
#include "Calculators/c_calculators.h"

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
		//static void config_chz();
		//static void config_cha();
		//static void config_chb();
		static c_configurator Configurator;
		static c_readers Reader;
		static c_calculators Calculator;
		//static void configure_encoder_quadrature();
		//static void configure_encoder_quadrature_w_z();
		//static void configure_test_channels();
		//static void get_rpm();
		////static void configure_disable_channels();
		//static void get_rpm_quad();
		//static void read_cha();
		//static void read_chb();
		//static void read_chz();
		static void test_channels();
		//static void read_quad();
		//static void read_quad_with_z();
		//static uint8_t get_active_channels();
	};
};

#endif

