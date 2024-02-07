/*
 * HMI_ECU.c
 *
 *      Author: Ayman_Mostafa
 */
#include"lcd.h"
#include"keypad.h"
#include "avr/io.h"
#include <util/delay.h>
#include"uart.h"
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
#define TIME_FOR_ERROR_MESSAGE      3
#define DOOR_HOLD_TIME              3
#define BUZZER_ON                   0xB0
#define BUZZER_OFF                  0xBF
#define BUZZER_ON_PERIOD			60
#define THERE_IS_PASSWORD_OR_NO     0x09
#define THERE_IS_PASSWORD           0x08
#define THERE_IS_NO_PASSWORD        0x07
#define TRIES_NUMBER                3

/*Global variables*/
uint8 password[MAX_DIGITS]={0};
uint8 password_check[MAX_DIGITS]={0};
uint8 g_commandRececived=0;
uint8 g_ticks=0;

/*this function for first step*/
void Step1_Create_System_Password()
{
	/*The LCD should display “Please Enter Password” like that: */
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString(" Plz Enter Pass:");
	LCD_moveCursor(1,0);
	/*Enter a password consists of 5 numbers, Display * in the screen for each number.*/
	for(uint8 i=0;i<MAX_DIGITS;i++)
	{
		password[i]=KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	/*wait for Press enter button*/
	KEYPAD_getPressedKey();
	_delay_ms(300);
	/*Ask the user to renter the same password for confirmation by display this message "Please re-enter the same Pass":*/
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("Plz Re-enter The");
	LCD_moveCursor(1,0);
	LCD_displayString("Same Pass: ");
	/*Enter a password consists of 5 numbers, Display * in the screen for each number. */
	for(uint8 i=0;i<MAX_DIGITS;i++)
	{
		password_check[i]=KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	/*wait for Press enter button*/
	KEYPAD_getPressedKey();
	_delay_ms(300);

}

/* function to display options Open the Door or Change Pass*/
void Step2_Main_Options(void)
{
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString(" + : Open Door");
	LCD_moveCursor(1,0);
	LCD_displayString(" - : Change Pass ");
}

/*function to display enter password in lcd and take password from user*/
void takePasswordFromUser(void)

{
	/*The LCD should display “Please Enter Password” like that: */
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString(" Plz Enter Pass:");
	LCD_moveCursor(1,0);
	/*Enter a password consists of 5 numbers, Display * in the screen for each number.*/
	for(uint8 i=0;i<MAX_DIGITS;i++)
	{
		password[i]=KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	/*wait for Press enter button*/
	KEYPAD_getPressedKey();
	_delay_ms(300);

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
	/*variable to count user's tries in entering password  */
	uint8 try=0;
	/*variable takes + or - values*/
	uint8 temp=0;
	// Enable global interrupts
	SREG=1<<7;
	//select settings for uart
	UART_ConfigType uart_config_1={EIGHT_BITS,DISABLED,ONE_BITS,9600};
	UART_init(&uart_config_1);
	//select settings for LCD
	LCD_init();

	while(1)
	{
		/*ask Control if there is password or no */
		UART_sendByte(THERE_IS_PASSWORD_OR_NO);
		/*waiting for CONTROL to answer */
		g_commandRececived=UART_recieveByte();
		if(g_commandRececived==THERE_IS_NO_PASSWORD)
		{
			/*Step1 – Create a System Password*/
			Step1_Create_System_Password();
			/*send the password*/
			UART_sendByte(SENDING_FIRST_PASSWORD);
			UART_sendArrayOfByte(password,MAX_DIGITS);
			/*send the check password */
			UART_sendByte(SENDING_SECOND_PASSWORD);
			UART_sendArrayOfByte(password_check,MAX_DIGITS);

			//receive command from conrol_ECU (matched or not)
			g_commandRececived=UART_recieveByte();

			//If the two passwords are unmatched then repeat step 1 again.
			if(g_commandRececived==MISMATCHED)
			{
				//display ERROR message,try again
				LCD_clearScreen();
				LCD_moveCursor(0,0);
				LCD_displayString("   Mismatched");
				LCD_moveCursor(1,0);
				LCD_displayString("   Try Again");
				DelaySecondTimer1(TIME_FOR_ERROR_MESSAGE);
				Step1_Create_System_Password();
			}
		}
		else
		{

			Step2_Main_Options();

			/*wait for Press enter button(+ or -)*/
			temp=KEYPAD_getPressedKey();
			_delay_ms(300);

			//send Control_ECU the command
			switch(temp)
			{
			case '+':
				/*Display enter the pass to open the door and take the pass from user*/
				takePasswordFromUser();
				/*Send Command to Control_ECU to check the entered password*/
				UART_sendByte(OPEN_DOOR_MODE);
				/*Send the entered password*/
				UART_sendArrayOfByte(password,MAX_DIGITS);
				/*Received from Control_ECU the result from comparing two passwords*/
				g_commandRececived=UART_recieveByte();
				if(g_commandRececived==MATCHED)
				{
					/*display a message on the screen “Door is Unlocking” for 15 Seconds */
					LCD_clearScreen();
					LCD_moveCursor(0,0);
					LCD_displayString("    Door is");
					LCD_moveCursor(1,0);
					LCD_displayString("   Unlocking");

					DelaySecondTimer1(TIME_FOR_UNLOKING_THE_DOOR);
					/*display a message on the screen “Door is Unlocking” for 3 Seconds */
					LCD_clearScreen();
					LCD_moveCursor(0,0);
					LCD_displayString("  Door is Open");
					DelaySecondTimer1(DOOR_HOLD_TIME);
					/*display a message on the screen “Door is locking” for 15 Seconds */
					LCD_clearScreen();
					LCD_moveCursor(0,0);
					LCD_displayString("    Door is");
					LCD_moveCursor(1,0);
					LCD_displayString("     locking");
					DelaySecondTimer1(TIME_FOR_LOKING_THE_DOOR);
				}
				else
				{
					/*increase the number of tries*/
					try++;
					if(try<TRIES_NUMBER)
					{
						//display ERROR message,try again
						LCD_clearScreen();
						LCD_moveCursor(0,0);
						LCD_displayString("   Mismatched");
						LCD_moveCursor(1,0);
						LCD_displayString("   Try Again");
						DelaySecondTimer1(TIME_FOR_ERROR_MESSAGE);
					}
					if(try==TRIES_NUMBER)
					{
						/* when numbers of tries=3 turn on the Buzzer*/
						UART_sendByte(BUZZER_ON);
						/*Display error message on LCD for 1 minute*/
						LCD_clearScreen();
						LCD_moveCursor(0,0);
						LCD_displayString(" ERROR MESSAGE");
						DelaySecondTimer1(BUZZER_ON_PERIOD);
						try=0;
					}
					else
						/* else turn off the Buzzer*/
						UART_sendByte(BUZZER_OFF);
				}
				break;

			case '-':
				/*take password from user to make him able to change password later*/
				takePasswordFromUser();
				/*Select the Mode*/
				UART_sendByte(CHANGE_PASSWORD);
				/*Send the entered password*/
				UART_sendArrayOfByte(password,MAX_DIGITS);
				/*Received from Control_ECU the result from comparing two passwords*/
				g_commandRececived=UART_recieveByte();
				if(g_commandRececived==MISMATCHED)
				{
					/*increase the number of tries*/
					try++;
					if(try<TRIES_NUMBER)
					{
						//display ERROR message,try again
						LCD_clearScreen();
						LCD_moveCursor(0,0);
						LCD_displayString("   Mismatched");
						LCD_moveCursor(1,0);
						LCD_displayString("   Try Again");
						DelaySecondTimer1(TIME_FOR_ERROR_MESSAGE);
					}
					if(try==TRIES_NUMBER)
					{
						/* when numbers of tries=3 turn on the Buzzer*/
						UART_sendByte(BUZZER_ON);
						/*Display error message on LCD for 1 minute*/
						LCD_clearScreen();
						LCD_moveCursor(0,0);
						LCD_displayString(" ERROR MESSAGE");
						DelaySecondTimer1(BUZZER_ON_PERIOD);
						try=0;
					}
					else
						/* else turn off the Buzzer*/
						UART_sendByte(BUZZER_OFF);
				}
				break;
			}
		}


	}

}

