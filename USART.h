/*
 * usart.h
 *
 * Created:	 24/02/2015 19:55:35
 * Author:	 Vinícius Knabben
 *
 * Info:	ATmega Datasheet
 */

#ifndef USART_H_
#define USART_H_

#include "MAIN_DEFINITIONS.h"
#include <stdio.h>
#include <avr/interrupt.h>

#define BAUD 9600	  // Baud Rate
#define MYUBRR	F_CPU/16/BAUD-1

void USART_Init(void);
/*stream Handlers*/
int USART_Send_Byte(char c, FILE* stream);
//int USART_Receive_Byte(FILE* stream);

/*read from ring buffer*/
int USART_Read_Byte(unsigned char * d);

int USART_Receive(char* d);

unsigned int USART_Available_Bytes(void);

// interrupts routine
ISR(USART_RX_vect);


#endif /* USART_H_ */