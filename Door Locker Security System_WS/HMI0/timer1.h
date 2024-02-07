/*
 * timer1.h
 *
 *      Author: Ayman_Mostafa
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                       definitions                                    *
 *******************************************************************************/

typedef enum
{
	NO_CLOCK,NO_PRESCALING,PRESCALE_8,PRESCALE_64,PRESCALE_256,PRESCALE_1024
}Timer1_Prescaler;

typedef enum
{
	NORMAL_MODE,COMPARE_MODE=4
}Timer1_Mode;

typedef struct {
 uint16 initial_value;
 uint16 compare_value; // it will be used in compare mode only.
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
} Timer1_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description
⮚ Function to initialize the Timer driver*/
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*Description
⮚ Function to disable the Timer1.*/
void Timer1_deInit(void);


/*Description
⮚ Function to set the Call Back function address.*/
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
