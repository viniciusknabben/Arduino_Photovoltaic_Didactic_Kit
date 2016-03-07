/*
 * PWM.h
 *
 * Created: 25/02/2015 9:27:55
 * Author: Vinicius Knabben
 *
 */

#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include "MAIN_DEFINITIONS.h"
#include <avr/interrupt.h>

#define ServoPWM_pin PC5
#define ServoPWM_DDR DDRC
#define ServoPWM_PORT PORTC

#define F_PWM 400	// PWM frequency
#define PS 1		// Pre scaler
#define TOP (F_CPU/(PS*F_PWM))-1	//{ PS = 1 } ; { F_PWM = 400 Hz }

#define ServoPos(ang)  (14250/90)*ang + 15500

void PWM_Init (void);

void PWM_SetCount(unsigned int count);
void PWM_SetVoltage(float v);

void PWM_ServoPos(unsigned int angle);
void PWM_ServoAnalogIn(unsigned int input);
ISR(TIMER1_COMPB_vect);
ISR(TIMER1_OVF_vect);


#endif // PWM_H_INCLUDED
