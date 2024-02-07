/*
 * timer1.c
 *
 *      Author: Ayman_Mostafa
 */

#include "avr/io.h"
#include "timer1.h"
#include "common_macros.h"
#include <avr/interrupt.h>


static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                              ISR                                    *
 *******************************************************************************/
/*ISR FOR COMPORE MODE  */
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*ISR FOR OVERFLOW MODE  */
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}

}

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description
⮚ Function to initialize the Timer driver*/
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* TCCR1A SETTING
	 * Normal port operation, OC1A/OC1B disconnected COM1A1/COM1B1=0 and COM1A0/COM1B0=0
	 * FOC1A=1 FOC1B=0 for non-PWM mode*/
	TCCR1A=0x08;

	//Select the prescaler
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);

	//Select the TIMER1 MODE
	switch(Config_Ptr->mode)
	{
	case NORMAL_MODE:
		CLEAR_BIT(TCCR1A,WGM10);
		CLEAR_BIT(TCCR1A,WGM11);
		CLEAR_BIT(TCCR1B,WGM12);
		CLEAR_BIT(TCCR1B,WGM13);
		//put the initial value in TCNT1
		TCNT1=Config_Ptr->initial_value;
		//Enable Overflow Interrupt
		SET_BIT(TIMSK,TOIE1);
		break;

	case COMPARE_MODE:
		CLEAR_BIT(TCCR1A,WGM10);
		CLEAR_BIT(TCCR1A,WGM11);
		SET_BIT(TCCR1B,WGM12);
		CLEAR_BIT(TCCR1B,WGM13);
		//put the compare value in OCR1A
		TCNT1 = 0;
		OCR1A=Config_Ptr->compare_value;
		//Enable Output Compare A Match Interrupt
		SET_BIT(TIMSK,OCIE1A);
		break;
	}
}

/*Description
⮚ Function to disable the Timer1.*/
void Timer1_deInit(void)
{
	TCCR1A=0;
	TCCR1B=0;
	OCR1A=0;
	TCNT1=0;
	TIMSK=TIMSK&0xC3;
	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;
}

/*Description
⮚ Function to set the Call Back function address.*/
void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr=a_ptr;
}
