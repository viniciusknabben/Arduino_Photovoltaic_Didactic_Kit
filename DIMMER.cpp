/*
 * dimmer.cpp
 *
 * Created: 08/09/2014 10:30:36
 *  Author: Vinicius Knabben
 */


#include "DIMMER.h"

volatile uint16_t intensity;

void DIMMER_Init(void)
{
    //Setup I/O
    set_bit(dimm_dreg,dimm_trigg); // 0 = Input / 1 = Output
    clr_bit(dimm_port,dimm_trigg); // Closes "dimmer_trigg"
    //

    //Setup External Interrupts INT0 for zero cross detection
    EICRA = 0x02; // Rising Edge Interrupt on INT0

    //Setup Timer2 -> 8bits -> max count = 255
    TCCR2B = 0x00;        //Disable Timer2
    TIFR2  = 0x00;        //Clean interrupts flags
    TIMSK2 = 0x01;        //Enable Overflow Interruption
    TCCR2A = 0x00;        //No wave generation mode
	
	DIMMER_Disable();
	intensity = 0;
}

void DIMMER_Enable(void)
{
	EIMSK = 1<<INT0; // Enable INT0
}

void DIMMER_Disable(void)
{
	EIMSK = 0<<INT0; // Disable INT0
}

void DIMMER_Intensity(uint16_t i)
{
	if(i<0)
		i=0;
	if(i>100)
		i=100;
		
    intensity = i;
}

ISR(INT0_vect) // Zero Cross detection
{
	TCNT2 = 170+(intensity/2);
	TCCR2B = 0x07; // Start timer2 prescaler with fcpu/1024
}

ISR(TIMER2_OVF_vect) // Trigger
{
	TCCR2B = 0x00; // Disable Timer2
	set_bit(dimm_port,dimm_trigg);
	_delay_us(10);
	clr_bit(dimm_port,dimm_trigg);
}
