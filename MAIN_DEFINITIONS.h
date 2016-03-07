#ifndef MAIN_DEFINITIONS_H_INCLUDED
#define MAIN_DEFINITIONS_H_INCLUDED

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

/* Define debug to debug mode */
#define debug

#define	set_bit(y,bit)	(y|=(1<<bit))	// set bit x from byte Y
#define	clr_bit(y,bit)	(y&=~(1<<bit))	// clear bit x from byte Y
#define cpl_bit(y,bit) 	(y^=(1<<bit))	// toggle bit x from byte Y
#define tst_bit(y,bit) 	(y&(1<<bit))	// test bit

typedef struct
{
	unsigned char mac_addr[6];
	unsigned char gtw_addr[4];
	unsigned char sub_mask[4];
	unsigned char ip_addr[4];
	uint16_t port;
	
}network_parameters_t;

#define np	200					// Number of points.
#define COEF_V 0.0244140		// Voltage conversion coef (V). -> (5/1024) /(G)		- G = 0,2
#define COEF_I 0.0003963		// Current conversion coef (A). -> (5/1024) /(Rs*G)		- G = 56 / Rs = 0,22R
#define COEF_T 0.097652			// Temperature conversion coef (°C).

#define MAX_LCD_CODES 2

typedef struct
{
	float vcoef;
	float icoef;
	float tcoef;
	unsigned int vp[np];
	unsigned int ip[np];
	unsigned int tp;
	
}analysis_pkg_t;

#define DATA_PKG_SIZE sizeof(analysis_pkg_t) //	414 bytes	



/*
	PINOUT:
	
	Atmega328 / ArduinoUNO / Function
			
	PORTB:	PB0 -  8 - NC
			PB1 -  9 - PWM PAINEL (OC1A)
			PB2 - 10 - SPI SS 
			PB3 - 11 - SPI MOSI
			PB4 - 12 - SPI MISO
			PB5 - 13 - SPI SCK
			PB6 -  x - XTAL
			PB7 -  x - XTAL
			
	PORTC:	PC0 - A0 - ADC VP
			PC1 - A1 - ADC IP
			PC2 - A2 - ADC TP
			PC3 - A3 - LCD E
			PC4 - A4 - LCD RS
			PC5 - A5 - PWM SERVO( Software PWM )
			PC6 - RESET

	PORTD:	PD0 - 0 - USART RX
			PD1 - 1 - USART TX
			PD2 - 2 - DIMMER INT0  
			PD3 - 3 - DIMMER trigger
			PD4 - 4 - LCD data0 
			PD5 - 5 - LCD data1
			PD6 - 6 - LCD data2	
			PD7 - 7 - LCD data3
			
			
	PINOUT:
			
	Atmega2560 / ArduinoMEGA Function
			
	PORTB:	PB0 - 52 - SPI SS 
			PB1 - 53 - SPI SCK
			PB2 - 51 - SPI MOSI 
			PB3 - 50 - SPI MISO
			
						
	PORTC:	PC0 - A0 - ADC VP
			PC1 - A1 - ADC IP
			PC2 - A2 - ADC TP
			PC3 - A3 - LCD E
			PC4 - A4 - LCD RS
			PC5 - A5 - PWM SERVO( Software PWM )
			PC6 - RESET

	PORTD:	PD0 - 0 - USART RX
			PD1 - 1 - USART TX
			PD2 - 2 - DIMMER INT0
			PD3 - 3 - DIMMER trigger
			PD4 - 4 - LCD data0
			PD5 - 5 - LCD data1
			PD6 - 6 - LCD data2
			PD7 - 7 - LCD data3
*/

#endif // MAIN_DEFINITIONS_H_INCLUDED
