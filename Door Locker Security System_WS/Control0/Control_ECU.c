/*
 * Control_ECU.c
 *
 *      Author: Ayman_Mostafa
 */

#include "external_eeprom.h"
#include "buzzer.h"
#include "dc_motor.h"
#include "avr/io.h"
#include"uart.h"
#include "twi.h"
#include <util/delay.h>
#include "timer1.h"



#define MAX_DIGITS 					5
#define MATCHED 					1
#define MISMATCHED 					0
#define MC_ADDRESS 					0x0E1
#define EEPROM_FIRST_ADDRESS_VALUE  0x10
#define SENDING_FIRST_PASSWORD      0x0F
#define SENDING_SECOND_PASSWORD     0x0E
#define OPEN_DOOR_MODE              0x0D
#define CHANGE_PASSWORD		        0x0C
#define MAX_SPEED_FOR_DC_MOTER		100
#define CTC_VALUE_FOR_ONE_SECOND 	7813
#define CTC_INITIAL_VALUE 			0
#define TIME_FOR_UNLOKING_THE_DOOR  15
#define TIME_FOR_LOKING_THE_DOOR    15
#define DOOR_HOLD_TIME              3
#define BUZZER_ON                   0xB0
#define BUZZER_OFF                  0xBF
#define BUZZER_ON_PERIOD			60
#define THERE_IS_PASSWORD_OR_NO     0x09
#define THERE_IS_PASSWORD           0x08
#define THERE_IS_NO_PASSWORD        0x07



uint8 password[MAX_DIGITS]={0};
uint8 password_check[MAX_DIGITS]={0};
uint8 g_commandRececived=0;
uint8 g_currentMode=0;
uint8 g_ticks=0;
uint8 g_passwordSatate=THERE_IS_NO_PASSWORD;

/*Saving password in EEPROM*/
void savePasswordToEEPROM(void)
{
	for(uint8 i=0;i<MAX_DIGITS;i++)
	{
		EEPROM_writeByte(EEPROM_FIRST_ADDRESS_VALUE + i, password[i]);
		_delay_ms(20);
	}
}

void readPasswordFromEEPROM(uint8*arr)
{
	for(uint8 i=0;i<MAX_DIGITS;i++)
	{
		EEPROM_readByte(EEPROM_FIRST_ADDRESS_VALUE+i,password[i]);
		_delay_ms(20);
	}
}

uint8 checkTwoArray(uint8*arr1,uint8*arr2,uint8 length)
{
	for(uint8 i=0;i<length;i++)
	{
		if(arr1[i]!=arr2[i])
		{
			return MISMATCHED;
		}
	}
	return MATCHED;
}

/* this function is executed each 1 second*/
void countOneSecond()
{
	g_ticks++;
}


void DelaySecondTimer1(uint8 timeSec)
{
	/* Select the configuration For TIMER1 */
	Timer1_ConfigType Config_Ptr1={CTC_INITIAL_VALUE,CTC_VALUE_FOR_ONE_SECOND,PRESCALE_1024,COMPARE_MODE};
	/* Set the call back function */
	Timer1_setCallBack(countOneSecond);
	/*Configuration For TIMER1 */
	Timer1_init(&Config_Ptr1);
	/* waiting for 15 seconds until the door is unlocking */
	while (g_ticks < timeSec);
	g_ticks = 0;
	/* stop the timer1 */
	Timer1_deInit();
}



int main(void)
{
	// Enable global interrupts
	SREG=1<<7;
	//select settings for uart
	UART_ConfigType uart_config_1={EIGHT_BITS,DISABLED,ONE_BITS,9600};
	UART_init(&uart_config_1);
	/* select the configuration of TWI */
	TWI_ConfigType twi_config_1 ={MC_ADDRESS, FAST_MODE_400_KB_PER_SEC};
	TWI_init(&twi_config_1);
	//initiation
	Buzzer_init();
	DcMotor_Init();


	while(1)
	{
		/* blocking and wait for order from HMI_ECU */
		g_currentMode = UART_recieveByte();

		switch(g_currentMode)
		{

		case THERE_IS_PASSWORD_OR_NO:
			UART_sendByte(g_passwordSatate);
			break;

		case SENDING_FIRST_PASSWORD:
			UART_recieveArrayOfByte(password,MAX_DIGITS);
			break;

		case SENDING_SECOND_PASSWORD:
			UART_recieveArrayOfByte(password_check,MAX_DIGITS);
			//check the two password and then send the result to HMI_ECU
			if(checkTwoArray(password,password_check,MAX_DIGITS))
			{
				UART_sendByte(MATCHED);
				savePasswordToEEPROM();
				g_passwordSatate=THERE_IS_PASSWORD;
			}
			else
			{
				UART_sendByte(MISMATCHED);
			}
			break;

		case OPEN_DOOR_MODE:
			/*Control_ECU receive password from HMI_ECU  */
			UART_recieveArrayOfByte(password,MAX_DIGITS);
			/*Read PASSWORD From EEPROM*/
			readPasswordFromEEPROM(password_check);
			if(checkTwoArray(password,password_check,MAX_DIGITS))
			{
				UART_sendByte(MATCHED);
				/*rotates motor for 15-seconds CW and display a message on the screen"Door is Unlocking"*/
				DcMotor_Rotate(CW,MAX_SPEED_FOR_DC_MOTER);
				/* waiting 15 seconds for the door to open  */
				DelaySecondTimer1(TIME_FOR_UNLOKING_THE_DOOR);
				/* Stop the motor */
				DcMotor_Rotate(STOP, 0);
				/* waiting for 3 seconds the hold period of the door */
				DelaySecondTimer1(DOOR_HOLD_TIME);
				/* turn on motor at max speed with anti clock wise direction */
				DcMotor_Rotate(A_CW,MAX_SPEED_FOR_DC_MOTER);
				/* waiting 15 seconds for the door to lock  */
				DelaySecondTimer1(TIME_FOR_UNLOKING_THE_DOOR);
				/* stop the motor */
				DcMotor_Rotate(STOP, 0);
			}
			else
			{
				/*send to HMI_ECU that passwords are mismatched*/
				UART_sendByte(MISMATCHED);
				/* wait HMI_ECU to take action to Turn on or Turn off the Buzzer */
				g_commandRececived=UART_recieveByte();
				if (g_commandRececived== BUZZER_ON)
				{
					/* turn on the buzzer */
					Buzzer_on();
					/* waiting for 1 minute */
					DelaySecondTimer1(BUZZER_ON_PERIOD);
					/* turn off the buzzer */
					Buzzer_off();
				}
			}
			break;

		case CHANGE_PASSWORD:
			/*Control_ECU receive password from HMI_ECU  */
			UART_recieveArrayOfByte(password,MAX_DIGITS);
			/*Read PASSWORD From EEPROM*/
			readPasswordFromEEPROM(password_check);
			/*checking between them*/
			if(checkTwoArray(password,password_check,MAX_DIGITS))
			{
				UART_sendByte(MATCHED);
				//now there is no password for system
				g_passwordSatate=THERE_IS_NO_PASSWORD;
			}
			else
			{
				/*send to HMI_ECU that passwords are mismatched*/
				UART_sendByte(MISMATCHED);
				/* wait HMI_ECU to take action to Turn on or Turn off the Buzzer */
				g_commandRececived=UART_recieveByte();
				if (g_commandRececived== BUZZER_ON)
				{
					/* turn on the buzzer */
					Buzzer_on();
					/* waiting for 1 minute */
					DelaySecondTimer1(BUZZER_ON_PERIOD);
					/* turn off the buzzer */
					Buzzer_off();
				}
			}
			break;
		}
	}
}
