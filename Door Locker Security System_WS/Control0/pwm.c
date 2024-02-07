/*
 * pwm.c
 *
 *      Author: Ayman_Mostafa
 */
#include "avr/io.h"
#include "pwm.h"
#include "common_macros.h"
#include "gpio.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
• Description:
➢ The function responsible for trigger the Timer0 with the PWM Mode.
➢ Setup the PWM mode with Non-Inverting.
➢ Setup the prescaler with F_CPU/8.
➢ Setup the compare value based on the required input duty cycle
➢ Setup the direction for OC0 as output pin through the GPIO driver.
➢ The generated PWM signal frequency will be 500Hz to control the DC
Motor speed
 */
void PWM_Timer0_Start(uint8 duty_cycle)
{
	/* configure the timer*/
	/* Set Timer Initial Value to 0*/
	TCNT0 = 0;
	/*Fast PWM mode FOC0=0 (Active only when the WGM00 bit specifies a non-PWM mode)*/
	CLEAR_BIT(TCCR0,FOC0);
	/*Fast PWM Mode WGM01=1 & WGM00=1*/
	SET_BIT(TCCR0,WGM00);
	SET_BIT(TCCR0,WGM01);
	/*Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1*/
	SET_BIT(TCCR0,COM01);
	CLEAR_BIT(TCCR0,COM00);
	/*Clock = F_CPU/8 CS00=0 CS01=1 CS02=0*/
	CLEAR_BIT(TCCR0,CS00);
	SET_BIT(TCCR0,CS01);
	CLEAR_BIT(TCCR0,CS02);
	/*Set Compare value*/
	OCR0  =(uint8)((uint16)(duty_cycle*255) /100);
	/*Set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.*/
	GPIO_setupPinDirection(PORTB_ID,PIN3_ID,PIN_OUTPUT);

}
