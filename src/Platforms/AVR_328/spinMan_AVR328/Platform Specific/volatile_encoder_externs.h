
#include <stdint-gcc.h>
#include "c_encoder_avr_328.h"
//I just learned this nifty macro trick for externs.. im trying it out to see if i like this coding style..
#ifdef __ENCODER_VOLATILES__

	//Whatever cpp defines __ENCODER_VOALTILES__ AND includes this .h file will have variable defined as below:
	#define __EXTERN(type,name , value)     volatile type name = value //define it here
	__EXTERN(uint16_t,enc_ticks_at_current_time,0);
	__EXTERN(uint32_t, enc_count, 0);
	__EXTERN(uint8_t, enc_active_channels, 0);
	__EXTERN(uint32_t, enc_ticks_per_rev, 0);
	__EXTERN(uint32_t, enc_sum_array[ENCODER_SUM_ARRAY_SIZE],{0});
	__EXTERN(uint8_t, enc_sum_array_head, 0);
	void (*Platform_Specific_HAL_Encoder_Vector)();
	

#else

	//Whatever cpp does NOT define __ENCODER_VOALTILES__ but includes this .h file will have externs defined as below:
	#define __EXTERN(type, name, value)     extern volatile type name; //declare it extern here
	__EXTERN(uint16_t,enc_ticks_at_current_time,0);
	__EXTERN(uint32_t, enc_count, 0);
	__EXTERN(uint8_t, enc_active_channels, 0);
	__EXTERN(uint32_t, enc_ticks_per_rev, 0);
	__EXTERN(uint32_t, enc_sum_array[ENCODER_SUM_ARRAY_SIZE],0);
	__EXTERN(uint8_t, enc_sum_array_head, 0);
	extern void (*Platform_Specific_HAL_Encoder_Vector)();

#endif
