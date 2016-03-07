#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

/*
	ADC Setup to read analog data from Solar Panel
	
	Voltage Gain = 1/5
	Current Gain = 
	Temperature Gain = 
*/

#include "MAIN_DEFINITIONS.h"

#define vp_ch 0x00
#define ip_ch 0x01
#define tp_ch 0x02

void ADC_Init(void);

uint16_t ADC_ReadVp();
uint16_t ADC_ReadIp();
uint16_t ADC_ReadTp();

#endif // ADC_H_INCLUDED
