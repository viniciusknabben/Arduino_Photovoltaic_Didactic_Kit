#include "ADC.h"

void ADC_Init(void)
{
	ADMUX  = 0b01000000;	//Extern ADC Voltage Reference = VCC(5V), ADLAR = 0 -> resultado de 8 bits em ADCL
	ADCSRA = 0b10000111;	//habilita ADC(bit7), prescaler = 128 (bits012)
	set_bit(DIDR0,0);		//desabilita pino PC0 como I/0, entrada do ADC0
	set_bit(DIDR0,1);		//desabilita pino PC1 como I/0, entrada do ADC1
	set_bit(DIDR0,2);		//desabilita pino PC2 como I/0, entrada do ADC2
}

uint16_t ADC_ReadVp()
{
	ADMUX &= 0xF0;
	ADMUX |= vp_ch;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

uint16_t ADC_ReadIp()
{
	ADMUX &= 0xF0;
	ADMUX |= ip_ch;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

uint16_t ADC_ReadTp()
{
	ADMUX &= 0xF0; // Change the Channel
	ADMUX |= tp_ch;
	ADCSRA |= (1<<ADSC); // Start the conversion
	while(ADCSRA & (1<<ADSC));
	return ADC;
}
