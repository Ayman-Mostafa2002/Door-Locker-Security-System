 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *

 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                       definitions                                    *
 *******************************************************************************/

typedef enum
{
	FIVE_BITS,SIX_BITS,SEVEN_BITS,EIGHT_BITS,NINE_BITS=7
}UART_BitData;

typedef enum
{
	DISABLED,EVEN_PARITY=2,ODD_PARITY=3
}UART_Parity;

typedef enum
{
	ONE_BITS,TWO_BITS
}UART_StopBit;


typedef struct{
 UART_BitData bit_data;
 UART_Parity parity;
 UART_StopBit stop_bit;
 uint32 baud_rate;
}UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for send array of byte to another UART device.
 */
void UART_sendArrayOfByte(const uint8 *data,uint8 length);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Functional responsible for receive array of bytes from another UART device.
 */
void UART_recieveArrayOfByte(uint8 *data,uint8 length);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */

void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
