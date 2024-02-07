/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr)
{
	uint16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);
	/************************** UCSRB Description *************************
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 ***********************************************************************/ 
	UCSRB = (1<<RXEN) | (1<<TXEN);

	/* URSEL   = 1 The URSEL must be one when writing the UCSRC*/
	UCSRC |= (1<<URSEL);

	/* when USBS= 0 One stop bit
	   when USBS= 1 TWO stop bits
	 */
	if(Config_Ptr->stop_bit)
		SET_BIT(UCSRC,USBS);
	else
		CLEAR_BIT(UCSRC,USBS);

	/* when UPM0= 0 and UPM1=0 DISABLED
	 * when UPM0= 0 and UPM1=1 EVEN_PARITY
	 * when UPM0= 1 and UPM1=1 ODD_PARITY
	 */
	switch(Config_Ptr->parity)
	{
	case DISABLED:
		CLEAR_BIT(UCSRC,UPM0);
		CLEAR_BIT(UCSRC,UPM1);
		break;
	case EVEN_PARITY:
		SET_BIT(UCSRC,UPM0);
		CLEAR_BIT(UCSRC,UPM1);
		break;
	case ODD_PARITY:
		SET_BIT(UCSRC,UPM0);
		SET_BIT(UCSRC,UPM1);
		break;
	}

	/* when UCSZ0= 0 , UCSZ1=0 ,and UCSZ2=0  5_bits
	   when UCSZ0= 1 , UCSZ1=0 ,and UCSZ2=0  6_bits
	   when UCSZ0= 0 , UCSZ1=1 ,and UCSZ2=0  7_bits
	   when UCSZ0= 1 , UCSZ1=1 ,and UCSZ2=0  8_bits
	   when UCSZ0= 1 , UCSZ1=1 ,and UCSZ2=1  9_bits
	 */
	switch(Config_Ptr->bit_data)
	{
	case FIVE_BITS:
		CLEAR_BIT(UCSRC,UCSZ0);
		CLEAR_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;
	case SIX_BITS:
		SET_BIT(UCSRC,UCSZ0);
		CLEAR_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;
	case SEVEN_BITS:
		CLEAR_BIT(UCSRC,UCSZ0);
		SET_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;
	case EIGHT_BITS:
		SET_BIT(UCSRC,UCSZ0);
		SET_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;
	case NINE_BITS:
		SET_BIT(UCSRC,UCSZ0);
		SET_BIT(UCSRC,UCSZ1);
		SET_BIT(UCSRB,UCSZ2);
		break;
	}

	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU / (Config_Ptr->baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	 *******************************************************************/
}


/*
 * Description :
 * Functional responsible for send array of byte to another UART device.
 */
void UART_sendArrayOfByte(const uint8 *data,uint8 length)
{
	for(uint8 i=0 ; i<length;i++)
	{
		UART_sendByte(data[i]);
	}
}


/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}

/*
 * Description :
 * Functional responsible for receive array of bytes from another UART device.
 */
void UART_recieveArrayOfByte(uint8 *data,uint8 length)
{
	for(uint8 i=0 ; i<length;i++)
	{
		data[i]=UART_recieveByte();
	}
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	 *******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
