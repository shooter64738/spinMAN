/*
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/
//https://www.avrfreaks.net/forum/no-asf-samd21-sercom-usart-rxc-interrupt-not-triggering
//https://www.avrfreaks.net/forum/no-asf-samd21-sercom3-tutorial-help

#include "c_serial_sam_d21.h"
#include "c_core_sam_d21.h"
#include "..\..\..\..\SAMD21_DFP\1.3.304\samd21a\include\sam.h"

#define COM_PORT_COUNT 1 //<--how many serial ports does this hardware have (or) how many do you need to use.
s_Buffer Hardware_Abstraction_Layer::Serial::rxBuffer[COM_PORT_COUNT];
PortGroup *Sercom_Port = &PORT->Group[0];

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	
	uint32_t ul_sr;
	switch (Port)
	{
		case 0:
		{
			uint32_t baud = 115200;
			
			// FBAUD = ( fREF/ S) (1 – BAUD/65,536)
			// FBAUD = baud frequency
			// fref – SERCOM generic clock frequency
			// S – Number of samples per bit
			// BAUD – BAUD register value
			uint64_t br = (uint64_t)65536 * (F_CPU - 16 * baud) / F_CPU; // Variable for baud rate

			Sercom_Port->DIRSET.reg = PORT_PA22;       // Set TX Pin direction to output
			Sercom_Port->PINCFG[PIN_PA22].reg |= PORT_PINCFG_INEN;    // Set TX Pin config for input enable (required for usart)
			Sercom_Port->PINCFG[PIN_PA22].reg |= PORT_PINCFG_PMUXEN;   // enable PMUX
			Sercom_Port->PMUX[PIN_PA22>>1].bit.PMUXE = PORT_PMUX_PMUXE_C_Val; // Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
			
			Sercom_Port->DIRCLR.reg = PORT_PA23;       // Set RX Pin direction to input
			Sercom_Port->PINCFG[PIN_PA23].reg |= PORT_PINCFG_INEN;    // Set RX Pin config for input enable
			Sercom_Port->PINCFG[PIN_PA23].reg &= ~PORT_PINCFG_PULLEN;   // enable pullup/down resistor
			Sercom_Port->PINCFG[PIN_PA23].reg |= PORT_PINCFG_PMUXEN;   // enable PMUX
			Sercom_Port->PMUX[PIN_PA23>>1].bit.PMUXO = PORT_PMUX_PMUXE_C_Val; // Set the PMUX bit (if pin is even, PMUXE, if odd, PMUXO)
			
			PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;      // Set the PMUX for SERCOM3 and turn on module in PM

			// Generic clock “SERCOM3_GCLK_ID_CORE” uses GCLK Generator 0 as source (generic clock source can be
			// changed as per the user needs), so the SERCOM3 clock runs at 8MHz from OSC8M
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM3_GCLK_ID_CORE) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

			// By setting the DORD bit LSB is transmitted first and setting the RXPO bit as 1
			// corresponding SERCOM PAD[1] will be used for data reception, PAD[0] will be used as TxD
			// pin by setting TXPO bit as 0, 16x over-sampling is selected by setting the SAMPR bit as
			// 0, Generic clock is enabled in all sleep modes by setting RUNSTDBY bit as 1,
			// USART clock mode is selected as USART with internal clock by setting MODE bit into 1.
			SERCOM3->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) | SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);
			
			// 8-bits size is selected as character size by setting the bit CHSIZE as 0,
			// TXEN bit and RXEN bits are set to enable the Transmitter and receiver
			SERCOM3->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);
			
			// baud register value corresponds to the device communication baud rate
			SERCOM3->USART.BAUD.reg = (uint16_t)br;

			// SERCOM3 peripheral enabled
			SERCOM3->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;

			//Set the Interrupt to use
			SERCOM3->USART.INTENSET.reg =
			SERCOM_USART_INTENSET_RXC	;	// Interrupt on received complete
			
			// Enable interrupts
			NVIC_EnableIRQ(SERCOM3_IRQn);
			//	NVIC_SetPriority (SERCOM3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  // set Priority
			
		}
		
	}
}

void Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	//Usart * port_usart = NULL;
	switch (Port)
	{
		case 0:
		{
			while (!(SERCOM3->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
			SERCOM3->USART.DATA.reg = byte;
		}
	}
	if (Port>0)
	{
		//if((port_usart->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY)
		//while((port_usart->US_CSR & US_CSR_TXRDY) != US_CSR_TXRDY)
		//{
		//}
		//port_usart->US_THR = (uint8_t)(byte);
		
		
	}
}

void Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(uint8_t Port, char Byte)
{
	if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head==RX_BUFFER_SIZE)
	{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head = 0;}
	
	//keep CR values, throw away LF values
	//if (Byte == 10)
	//return;
	
	Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] = Byte;
	
	if (Byte == 13)
	{
		
		Hardware_Abstraction_Layer::Serial::rxBuffer[Port].EOL++;
	}
	
	Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head++;

	if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head == Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Tail)
	{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].OverFlow=true;}
}

void Hardware_Abstraction_Layer::Serial::_outgoing_serial_isr(uint8_t Port, char Byte)
{
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head==RX_BUFFER_SIZE)
	//{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head = 0;}
	//
	////keep CR values, throw away LF values
	////if (Byte == 10)
	////return;
	//
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] = Byte;
	//
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] == 13)
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].EOL++;
	//
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head++;
	//
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head == Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Tail)
	//{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].OverFlow=true;}
}

void SERCOM3_Handler()
{
	if (SERCOM3->USART.INTFLAG.bit.RXC)
	{
		char Byte = SERCOM3->USART.DATA.reg;
		uint16_t rxData = SERCOM3->USART.DATA.reg;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(0,Byte);
	}
}