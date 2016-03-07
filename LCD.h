#ifndef _LCD_H
#define _LCD_H

#include "MAIN_DEFINITIONS.h"
#include <avr/interrupt.h> 


#define DATA_DDR_LCD		DDRD
#define DATA_PORT_LCD    	PORTD  	//4 bits de dados do LCD no PORTD 
#define nibble_dados		1		//0 para via de dados do LCD nos 4 LSBs do PORT empregado (Px0-D4, Px1-D5, Px2-D6, Px3-D7) 
									//1 para via de dados do LCD nos 4 MSBs do PORT empregado (Px4-D4, Px5-D5, Px6-D6, Px7-D7)
								
#define CTRL_DDR_LCD	DDRC								 
#define CTRL_PORT_LCD 	PORTC  	//PORT com os pinos de controle do LCD (pino R/W em 0).
#define E    			PC3     //pino de habilitação do LCD (enable)
#define RS   			PC4     //pino para informar se o dado é uma instrução ou caractere

#define tam_vetor	5	//número de digitos individuais para a conversão por ident_num()	 
#define conv_ascii	48	//48 se ident_num() deve retornar um número no formato ASCII (0 para formato normal)

//sinal de habilitação para o LCD
#define pulso_enable() 	_delay_us(1); set_bit(CTRL_PORT_LCD,E); _delay_us(1); clr_bit(CTRL_PORT_LCD,E); _delay_us(45)


void cmd_LCD(unsigned char c, char cd);

void LCD_Init();

void LCD_Write(const char *c);

void LCD_CS();

void LCD_Cursor_Pos(int linha,int coluna);

void LCD_ident_num(unsigned int valor,char *disp);

void LCD_Refresh(uint8_t state);

#endif

/*
	Comandos do LCD -> cmd_LCD(comando,0)
	
	
	Pagina 545

*/