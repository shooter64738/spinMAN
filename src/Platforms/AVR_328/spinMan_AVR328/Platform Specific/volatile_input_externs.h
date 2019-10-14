
#include <stdint.h>
//I just learned this nifty macro trick for externs.. im trying it out to see if i like this coding style..
#ifdef __INPUT_VOLATILES__
	//Whatever cpp defines __ENCODER_VOALTILES__ AND includes this .h file will have veriables defined as below:
	#define __EXTERN(type,name , value)     type name = value //define it here
	__EXTERN(uint32_t, tmr_count_ticks, 0);
	__EXTERN(uint16_t, pid_count_ticks, 0);
	__EXTERN(uint16_t, rpm_count_ticks, 0);
	__EXTERN(uint16_t, freq_interval, 0);
	__EXTERN(uint32_t, extern_input__time_count, 0);
	__EXTERN(uint16_t, _ref_enc_count, 0);
	__EXTERN(uint8_t, extern_input__intervals, 0);
#else
	//Whatever cpp does NOT define __ENCODER_VOALTILES__ but includes this .h file will have externs defined as below:
	#define __EXTERN(type, name, value)     extern type name; //declare it extern here
	__EXTERN(uint32_t, tmr_count_ticks, 0);
	__EXTERN(uint16_t, pid_count_ticks, 0);
	__EXTERN(uint16_t, rpm_count_ticks, 0);
	__EXTERN(uint16_t, freq_interval, 0);
	__EXTERN(uint32_t, extern_input__time_count, 0);
	__EXTERN(uint16_t, _ref_enc_count, 0);
	__EXTERN(uint8_t, extern_input__intervals, 0);
#endif


