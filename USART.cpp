/*
 * usart.cpp
 *
 * Created:	 24/02/2015 19:55:35
 * Author:	 Vinícius Knabben
 *
 * Info:	ATmega Datasheet
 */

#include "USART.h"
#include "RING_BUFFER.h"

static FILE mystdout;
static RingBuffer * usart_rx_rb;

void USART_Init(void)
{	
	unsigned int ubrr = MYUBRR; // USART Baud Rate Register value
	
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	/* Disable double speed */
	UCSR0A = 0;
	
	/* Enable receiver and transmitter and rx interrupt*/
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

	/* Set Asynch mode; Frame format = 8bits; Stop bits = 1bit; Parity = null */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	
	usart_rx_rb = create_RB();
	
	fdev_setup_stream(&mystdout, USART_Send_Byte, NULL, _FDEV_SETUP_WRITE);
	stdout=&mystdout;
}



int USART_Send_Byte(char data, FILE* stream)
{		
	while ( !( UCSR0A & (1<<UDRE0)) );	// Waits UDRE0 = 1 (transmission buffer clear)
	UDR0 = data; // Sending data (UDR0 = 0)
	
	return 0;
}

ISR(USART_RX_vect)
{
	if(!rb_full(usart_rx_rb))
	{
		rb_put(usart_rx_rb, UDR0);
	}
}

int USART_Read_Byte(unsigned char * d)
{
	if(rb_empty(usart_rx_rb))
		return 0;
		
	*d = rb_remove(usart_rx_rb);
	return 1;
	
}

unsigned int USART_Available_Bytes()
{
	return rb_size(usart_rx_rb);
}

/*
int USART_Receive_Byte(FILE* stream)
{	
	while( !(UCSR0A & (1<<RXC0)) ); //   when there is data to read RXC0 = 1
	return UDR0;
}
*/


int USART_Receive(char* d)
{
	unsigned char time_out=10;
	
	// Wait for data to be received //
	
	while ( (!(UCSR0A & (1<<RXC0)) ) && time_out ) //   when there is data to read (RXC0 = 1), or time out, break the loop
	{
		time_out--;
		_delay_ms(100);
		
	}

	if(time_out) // there is data
	{
		*d = UDR0;
		return 1;
	}
	else // no data
	return 0;
}

