/*
 * buzzer.h
 *
 *      Author: Ayman_Mostafa
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*  buzzer HW Ports and Pins */
#define BUZZER_PORT_ID              PORTA_ID
#define BUZZER_PIN_ID               PIN0_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description
  ⮚ Setup the direction for the buzzer pin as output pin through the GPIO driver.
  ⮚ Turn off the buzzer through the GPIO.
 */
void Buzzer_init(void);

/* Description
   ⮚ Function to enable the Buzzer through the GPIO.
*/
void Buzzer_on(void);

/* Description
   ⮚ Function to disable the Buzzer through the GPIO.
*/
void Buzzer_off(void);

#endif /* BUZZER_H_ */
