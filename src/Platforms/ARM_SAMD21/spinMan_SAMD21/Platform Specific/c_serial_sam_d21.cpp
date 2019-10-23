/* 
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_sam_d21.h"
#include "c_core_sam_d21.h"
#include "..\..\..\..\SAMD21_DFP\1.3.304\samd21a\include\sam.h"

#define COM_PORT_COUNT 1 //<--how many serial ports does this hardware have (or) how many do you need to use. 
s_Buffer Hardware_Abstraction_Layer::Serial::rxBuffer[COM_PORT_COUNT];


void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	
	uint32_t ul_sr;
	switch (Port)
	{
		case 0:
		{
			// ==> Pin configuration
			// Disable interrupts on Rx and Tx
			PIOA->PIO_IDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

			// Disable the PIO of the Rx and Tx pins so that the peripheral controller can use them
			PIOA->PIO_PDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

			// Read current peripheral AB select register and set the Rx and Tx pins to 0 (Peripheral A function)
			ul_sr = PIOA->PIO_ABSR;
			PIOA->PIO_ABSR &= ~(PIO_PA8A_URXD | PIO_PA9A_UTXD) & ul_sr;

			// Enable the pull up on the Rx and Tx pin
			PIOA->PIO_PUER = PIO_PA8A_URXD | PIO_PA9A_UTXD;

			// ==> Actual uart configuration
			// Enable the peripheral uart controller
			PMC->PMC_PCER0 = 1 << ID_UART;

			// Reset and disable receiver and transmitter
			UART->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

			// Set the baudrate
			UART->UART_BRGR = (F_CPU/BaudRate)/16; //46; // 84,000,000 / 16 * x = BaudRate (write x into UART_BRGR)

			// No Parity
			UART->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;

			// Disable PDC channel
			UART->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

			// Configure interrupts
			UART->UART_IDR = 0xFFFFFFFF;
			UART->UART_IER = UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME;

			// Enable UART interrupt in NVIC
			NVIC_EnableIRQ((IRQn_Type) ID_UART);

			// Enable receiver and transmitter
			UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
			break;
		}
		
	}
}

void Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	Usart * port_usart = NULL;
	switch (Port)
	{
		case 0:
		{
			//Wait until tx is ready.
			//while(!(UART->UART_SR & UART_SR_TXRDY))
			
			//if((UART->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
			//{
			//	return;
			//}

			// Send the character
			UART->UART_THR = byte;
			while(!((UART->UART_SR) & UART_SR_TXEMPTY)); // Wait for the tx to complete
			break;
		}
	}
	if (Port>0)
	{
		//if((port_usart->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY)
		while((port_usart->US_CSR & US_CSR_TXRDY) != US_CSR_TXRDY)
		{
		}
		port_usart->US_THR = (uint8_t)(byte);
		
		
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

void UART_Handler(void)
{
	uint32_t status = UART->UART_SR;
	//if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
	if((status & UART_SR_RXRDY))
	{
		char Byte = UART->UART_RHR;
		
		
		//UART->UART_THR = Byte;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(0,Byte);
	}
}