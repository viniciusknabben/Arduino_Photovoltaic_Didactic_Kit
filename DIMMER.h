/*
 * dimmer.h
 *
 * Created: 08/09/2014 10:30:36
 * Author: Vinicius Knabben
 *
 */

#ifndef DIMMER_H_INCLUDED
#define DIMMER_H_INCLUDED

#include "MAIN_DEFINITIONS.h"
#include <avr/interrupt.h>

#define dimm_dreg DDRD
#define dimm_port PORTD
#define dimm_trigg PD3 // output triacc

void DIMMER_Init(void);

void DIMMER_Enable(void);

void DIMMER_Disable(void);

void DIMMER_Intensity(uint16_t);

ISR(INT0_vect);

ISR(TIMER2_OVF_vect);

#endif // DIMMER_H_INCLUDED
