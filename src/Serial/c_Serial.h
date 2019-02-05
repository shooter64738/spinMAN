/*
* c_Seriali.h
*
* Created: 1/10/2018 10:33:23 PM
* Author: jeff_d
*/


#ifndef c_Serial_h
#define c_Serial_h
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "s_Buffer.h"
#include <stdint.h>
#include <stdint-gcc.h>
//#include "../rtc_c++.h"

#define F_CPU 16000000L
#define CR 13

//static s_Buffer rxBuffer[2];

class c_Serial
{
	//variables
	public:
	
	uint8_t EOL;
	
	protected:
	
	private:
	uint8_t _check_tx_port_data(char );
	uint8_t _check_rx_port_data();
	uint8_t _port;

	//functions
	public:
	c_Serial();
	c_Serial(uint8_t Port, uint32_t BaudRate);
	
	uint8_t Available();
	void ClearBuffer(void);
	void InitBuffer(void);
	char Get();
	char Peek();
	char Peek(uint8_t LookAhead);
	uint8_t HasEOL();
	void Write(const char*);
	void Write(char);
	void Write_ni(int16_t);
	void Write_ni(int16_t,uint8_t);
	void Write_nf(float);
	void print_int32(long n);
	void print_float(float n, uint8_t decimal_places);
	void print_float(float n);
	void print_string(const char *s);
	uint16_t DataSize ();
	
	//~c_Seriali();
	protected:
	private:
	void print_uint32_base10(uint32_t n);
	
};

#endif //__C_SERIALI_H__
