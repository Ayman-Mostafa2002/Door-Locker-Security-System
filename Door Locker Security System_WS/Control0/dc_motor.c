/*
 * dc_motor.c
 *
 *      Author: Ayman_Mostafa
 */
#include "avr/io.h"
#include "dc_motor.h"
#include "common_macros.h"
#include "gpio.h"
#include "pwm.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description
 * The Function responsible for setup the direction for the two
   motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver..
 */
void DcMotor_Init(void)
{
	/*  setup the direction for the two motor pins through the GPIO driver. */
	GPIO_setupPinDirection(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,PIN_OUTPUT);
	/* Stop at the DC-Motor at the beginning through the GPIO driver */
	GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
}

/*
 * Description
 * The function responsible for rotate the DC Motor CW/ or A-CW or
   stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	/*rotate the DC Motor CW/ or A-CW or stop the motor based on the state*/
	switch (state)
	{
	case STOP:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	case CW:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	case A_CW:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_HIGH);
		break;
	default:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	}
	//send the speed to PWM function
	PWM_Timer0_Start(speed);
}




