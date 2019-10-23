#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__) && !defined(__SAMD21G18AU__)
#define MSVC
#endif

#ifndef HARDWARE_DEF_H_
#define HARDWARE_DEF_H_

#if defined __AVR_ATmega328P__
#define F_CPU 16000000UL
#define PWM_RESOLUTION_BIT 16 //<--Used to keep pid output in an appropriate range.
#define MAX_PWM_VALUE UINT16_MAX  //<--This is used to determine highest pwm output
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0);
	#define __NOP() "nop";

#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/c_core_avr_328.h"
#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/c_outputs_avr_328.h"
#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/c_inputs_avr_328.h"
#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/c_serial_avr_328.h"
#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/Encoder/c_encoder_avr_328.h"
#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/c_storage_avr_328.h"
//#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/volatile_encoder_externs.h"
//#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/volatile_input_externs.h"
#endif

#ifdef __SAM3X8E__
#define F_CPU 84000000UL
#endif

#ifdef __SAMD21G18AU__ 
#define F_CPU 24000000UL
#define PWM_RESOLUTION_BIT 16 //<--Used to keep pid output in an appropriate range.
#define MAX_PWM_VALUE UINT16_MAX  //<--This is used to determine highest pwm output
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0);
#define __NOP() "nop";
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/c_core_sam_d21.h"
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/c_outputs_sam_d21.h"
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/c_inputs_sam_d21.h"
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/c_serial_sam_d21.h"
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/Encoder/c_encoder_sam_d21.h"
#include "Platforms/ARM_SAMD21/spinMan_SAMD21/Platform Specific/c_storage_sam_d21.h"
//#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/volatile_encoder_externs.h"
//#include "Platforms/AVR_328/spinMan_AVR328/Platform Specific/volatile_input_externs.h"
#endif

#ifdef MSVC
#define PWM_RESOLUTION_BIT 16 //<--Used to keep pid output in an appropriate range.
#define F_CPU 16000000
#define F_CPU_2 F_CPU/2
#define _TICKS_PER_MICROSECOND (F_CPU_2/1000000)
#include "Serial/c_Serial.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/Encoder/c_encoder_win.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/c_inputs_win.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/c_outputs_win.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/c_core_win.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/c_serial_win.h"
#include "Platforms/WIN32/spinMan_win32/Platform Specific/c_storage_win.h"
#endif
#endif