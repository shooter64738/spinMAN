/*
* c_Seriali.cpp
*
* Created: 1/10/2018 10:33:23 PM
* Author: jeff_d
*/

#include "c_Serial.h"
#include <avr/interrupt.h>
//#include "../numeric_converters.h"
//#include "../rtc_c++.h"
static s_Buffer rxBuffer[2];
// default constructor
c_Serial::c_Serial()
{
	
}

c_Serial::c_Serial(uint8_t Port, uint32_t BaudRate)
{
	this->_port=Port;
	InitBuffer();

	uint16_t UBRR0_value = 0;
	//uint32_t BaudRate = 115200;
	
	switch (this->_port)
	{
		case 0:
		{
			if (BaudRate < 57600)
			{
				UBRR0_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR0A &= ~(1 << U2X0); // 2x baud disable
			}
			else
			{
				UBRR0_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR0A |= (1 << U2X0);  // 2x baud enable
			}
			UBRR0H = UBRR0_value >> 8;
			UBRR0L = UBRR0_value;

			UCSR0B |= (1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0 );

			break;
		}
		#ifdef UCSR1A

		case 1:
		{
			if (BaudRate < 57600)
			{
				UBRR0_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR1A &= ~(1 << U2X1); // 2x baud disable
			}
			else
			{
				UBRR0_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR1A |= (1 << U2X1);  // 2x baud enable
			}
			UBRR1H = UBRR0_value >> 8;
			UBRR1L = UBRR0_value;
			
			UCSR1B |= (1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1);
			//UCSR1C =  (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
			//UCSR1C = ((1<<UCSZ00)|(1<<UCSZ01));
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			if(BaudRate < 57600)
			{
				UBRR0_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR2A &= ~(1 << U2X2); // 2x baud disable
			}
			else
			{
				UBRR0_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR2A |= (1 << U2X2);  // 2x baud enable
			}
			UBRR2H = UBRR0_value >> 8;
			UBRR2L = UBRR0_value;

			UCSR2B |= (1 << RXEN2 | 1 << TXEN2);// | 1 << RXCIE2);
			
			break;
		}
		#endif
		#ifdef UCSR3A
		
		case 3:
		{
			if (BaudRate < 57600)
			{
				UBRR0_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR3A &= ~(1 << U2X3); // 2x baud disable
			}
			else
			{
				UBRR0_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR3A |= (1 << U2X3);  // 2x baud enable
			}
			
			UBRR3H = UBRR0_value >> 8;
			UBRR3L = UBRR0_value;

			// flags for interrupts
			UCSR3B |= (1 << RXEN3 | 1 << TXEN3);// | 1 << RXCIE3);
			break;
		}
		#endif
	}
}

void c_Serial::ClearBuffer()
{
	//memset(rxBuffer[_port].Buffer,0,RX_BUFFER_SIZE);
	for (int i=0;i<RX_BUFFER_SIZE;i++)
	rxBuffer[_port].Buffer[i]=0;
}

void c_Serial::InitBuffer()
{
	rxBuffer[_port].Head=0;
	rxBuffer[_port].Tail=0;
	EOL=false;
	ClearBuffer();
}

char c_Serial::Get()
{
	
	
	char byte = rxBuffer[_port].Buffer[rxBuffer[_port].Tail];
	if (byte == 13)
	{
		rxBuffer[_port].EOL--;
		//rxBuffer[_port].Buffer[rxBuffer[_port].Tail]=0;//<--clear the CR byte.
	}
	
	
	rxBuffer[_port].Tail++;

	if (rxBuffer[_port].Tail == RX_BUFFER_SIZE)
	{
		rxBuffer[_port].Tail=0;
	}
	
	return byte;
}

char c_Serial::Peek()
{
	
	char byte = rxBuffer[_port].Buffer[rxBuffer[_port].Tail];
	//Write(byte);
	return byte;
}

char c_Serial::Peek(uint8_t LookAhead)
{
	if (rxBuffer[_port].Tail+LookAhead> rxBuffer[_port].Head)
	return 0;
	
	char byte = rxBuffer[_port].Buffer[rxBuffer[_port].Tail+LookAhead];
	//Write(byte);
	return byte;
}

uint8_t c_Serial::Available()
{
	
	if (rxBuffer[_port].Head != rxBuffer[_port].Tail )
	{
		//UDR0='A';
		return true;
		
	}
	return false;
	//return rxBuffer[_port].New;
}

uint8_t c_Serial::HasEOL()
{
	return rxBuffer[_port].EOL>0?true:false;
}

uint16_t c_Serial::DataSize()
{
	uint16_t distane_to_head =
	rxBuffer[_port].Head<rxBuffer[_port].Tail?rxBuffer[_port].Head+RX_BUFFER_SIZE:rxBuffer[_port].Head;
	return distane_to_head - rxBuffer[_port].Tail;
	
}

/*This sends the specified string. It will not return until transmission is complete*/
void c_Serial::Write(const char *Buffer)
{
	while (*Buffer!=0)
	Write(*Buffer++);
	//int size =256;
	//for (int i=0;i<size;i++)
	//Write(Buffer[i]);
}

void c_Serial::Write(char Buffer)
{
	_check_tx_port_data(Buffer);
}

void c_Serial::Write_ni(int16_t val) {
	unsigned char buf[5];
	if (val < 0) {
		this->Write('-');
		val *= -1;
	}
	int8_t ptr;
	for(ptr=0;ptr<5;++ptr) {
		buf[ptr] = (val % 10) + '0';
		val /= 10;
	}
	for(ptr=4;ptr>0;--ptr) {
		if (buf[ptr] != '0') break;
	}
	for(;ptr>=0;--ptr) {
		this->Write(buf[ptr]);
	}
}

void c_Serial::Write_ni(int16_t val,uint8_t Width)
{
	char Buffer[18];
	//num_to_string::integer_to_string(val,Buffer,Width);
	this->Write(Buffer);
}

void c_Serial::Write_nf(float val) {
	
	char Buffer[18]; //<-- Format of number is expected to be '+1234.5678'. the basic buffer needs to be 10 bytes
	//because the +- is always going to be there, and we can have 4 decimals. We also want the
	//units of the axis displayed as well. The 10th position is the units, the 11th position is a null
	//strcpy(Buffer,"00000.0000");
	
	//Convert the float value to a string, after it is converted into the appropriate unit value
	//num_to_string::float_to_string_no_sign(val,Buffer);
	this->Write(Buffer);
}

void c_Serial::print_string(const char *s)
{
	while (*s)
	this->Write(*s++);
	//this->Write(CR);
}

void c_Serial::print_uint32_base10(uint32_t n)
{
	if (n == 0)
	{
		Write('0');
		//this->Write(CR);
		return;
	}

	unsigned char buf[10];
	uint8_t i = 0;

	while (n > 0)
	{
		buf[i++] = n % 10;
		n /= 10;
	}

	for (; i > 0; i--)
	Write('0' + buf[i - 1]);

	//this->Write(CR);
}

void c_Serial::print_int32(long n)
{
	if (n < 0)
	{
		Write('-');
		print_uint32_base10(-n);
	}
	else
	{
		print_uint32_base10(n);
	}
	//this->Write(CR);
}

// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up
// techniques are actually just slightly slower. Found this out the hard way.
void c_Serial::print_float(float n)
{
	this->print_float(n,4);
}

void c_Serial::print_float(float n, uint8_t decimal_places)
{
	if (n < 0)
	{
		Write('-');
		n = -n;
	}

	uint8_t decimals = decimal_places;
	while (decimals >= 2)
	{ // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}
	if (decimals)
	{
		n *= 10;
	}
	n += 0.5; // Add rounding factor. Ensures carryover through entire value.

	// Generate digits backwards and store in string.
	unsigned char buf[13];
	uint8_t i = 0;
	uint32_t a = (long)n;
	while (a > 0)
	{
		buf[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}
	while (i < decimal_places)
	{
		buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}
	if (i == decimal_places)
	{ // Fill in leading zero, if needed.
		buf[i++] = '0';
	}

	// Print the generated string.
	for (; i > 0; i--)
	{
		if (i == decimal_places)
		{
			Write('.');
		} // Insert decimal point in right place.
		Write(buf[i - 1]);
	}
	//this->Write(CR);
}

uint8_t c_Serial::_check_tx_port_data(char ByteBuffer)
{
	switch (this->_port)
	{
		case 0:
		{
			while(! (UCSR0A & (1 << UDRE0)));
			UDR0 = ByteBuffer;
			break;
		}
		#ifdef UCSR1A
		case 1:
		{
			while(! (UCSR1A & (1 << UDRE1)));
			UDR1 = ByteBuffer;
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			while(! (UCSR2A & (1 << UDRE2))){}
			UDR2 = ByteBuffer;
			break;
		}
		#endif
		#ifdef UCSR3A
		case 3:
		{
			while(! (UCSR3A & (1 << UDRE3))){}
			UDR3 = ByteBuffer;
			break;
		}
		#endif
	}
	return false;
}

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	char Byte = UDR0;

	if (rxBuffer[0].Head==RX_BUFFER_SIZE)
	{rxBuffer[0].Head = 0;}

	
	//keep CR values, throw away LF values
	if (Byte == 10)
	return;
	
	rxBuffer[0].Buffer[rxBuffer[0].Head] = Byte;
	
	if (rxBuffer[0].Buffer[rxBuffer[0].Head] == 13)
	rxBuffer[0].EOL++;
	
	rxBuffer[0].Head++;

	if (rxBuffer[0].Head == rxBuffer[0].Tail)
	{rxBuffer[0].OverFlow=true;}
}
#endif

#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	char Byte = UDR0;

	if (rxBuffer[0].Head==RX_BUFFER_SIZE)
	{rxBuffer[0].Head = 0;}

	
	//keep CR values, throw away LF values
	if (Byte == 10)
	return;
	
	rxBuffer[0].Buffer[rxBuffer[0].Head] = Byte;
	
	if (rxBuffer[0].Buffer[rxBuffer[0].Head] == 13)
	rxBuffer[0].EOL++;
	
	rxBuffer[0].Head++;

	if (rxBuffer[0].Head == rxBuffer[0].Tail)
	{rxBuffer[0].OverFlow=true;}
}
#endif

#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	char Byte = UDR1;

	if (rxBuffer[1].Head==RX_BUFFER_SIZE)
	{rxBuffer[1].Head = 0;}

	//keep CR values, throw away LF values
	if (Byte == 10)
	return;
	
	rxBuffer[1].Buffer[rxBuffer[1].Head] = Byte;
	
	if (rxBuffer[1].Buffer[rxBuffer[1].Head] == 13)
	rxBuffer[1].EOL++;
	
	rxBuffer[1].Head++;

	if (rxBuffer[1].Head == rxBuffer[1].Tail)
	{rxBuffer[1].OverFlow=true;}
}
#endif
