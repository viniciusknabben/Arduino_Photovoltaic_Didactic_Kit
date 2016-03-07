/*
 * Kit_Mod2_ATmega328.cpp
 *
 * Created: 16/10/2014 02:17:50
 *  Author: Vinicius Knabben
 */ 

/**********************************************************
					
					INCT - EREEA
					
				IFSC - Sistemas Eletrônicos
					
			Kit Didatico Ftovoltaico Módulo 2
		

Ultima atualização em: 14/11/2015
**********************************************************/

#include "MAIN_DEFINITIONS.h"
#include "DIMMER.h"
#include "W5100.h"
#include "SOCKET_W5100.h"
#include "LCD.h"
#include "PWM.h"
#include "ADC.h"
#include "USART.h"


int networkConf(void);
void server_param_string(char * port_str, char * ip_str);

void refresh_LCD_init(void);
void enable_LCD_refresh(void);
void disable_LCD_refresh(void);
void refresh_LCD(int code);
ISR(TIMER0_OVF_vect);

char string_ip[16];								// variable to hold ip value as a string
char string_port[6];							// variable to hold port value as a string

//Main----------------
void simulation(analysis_pkg_t* pkg, uint16_t ang, uint16_t lum);
void analysis (analysis_pkg_t* pkg, uint16_t ang, uint16_t lum);
//--------------------

int main(void)
{	
	
	uint16_t* uint_ptr;								// pointer to receive 16bits data
	uint16_t ang;									// variable to hold the user angle value
	uint16_t lum;									// variable to hold the user luminosity value
	char string_aux[5];
	uint8_t rx_buf[10];								// Ethernet receive buffer					
	
	analysis_pkg_t analysis_data;					// Ethernet send buffer
	
	analysis_data.vcoef = COEF_V;
	analysis_data.icoef = COEF_I;
	analysis_data.tcoef = COEF_T;
	
	// Initializations //
	
	LCD_Init();		// Initialize LCD functions
	refresh_LCD_init();
	
	USART_Init(); // Set Baud Rate on header file
	_delay_ms(1000);
	#ifdef debug
		printf("Initializing on Debug Mode\n\n");
	#endif
	
	DIMMER_Init();	// Initialize dimmer functions 
	
	ADC_Init();		// Initialize A/D Conversions
	
	PWM_Init();		// Initialize Wave generators
	
	reconfig: // reconfig return label

	cli(); // disable all interrupts
	
	while(!W5100_Init()) // Try to Initialize W5100 until there is no network parameters 
	{
		printf("Waiting for network parameters\n");
		
		while(USART_Available_Bytes() < sizeof(network_parameters_t)); // check for incoming Serial data for network configuration
			
		networkConf(); // try to configure network parameters
	}
	
	Socket_W5100 s1 = Socket_W5100();
	
	s1.setPort(W5100_Port_Number());
	
	server_param_string(&string_port[0],&string_ip[0]);
	
	sei(); // enable all interrupts
	
	PWM_ServoPos(90);
	
	while(1)
	{		
		
		if(s1.sockStat() == SOCK_ESTABLISHED)
		{
			disable_LCD_refresh();
			
			LCD_CS();
			LCD_Write("Connected...");
			_delay_ms(1000);
			
			
			if(s1.receiveSize()) // returns 0 if there is no data...
			{
				s1.socketReceiveData(rx_buf,s1.receiveSize());
				uint_ptr = (uint16_t*) rx_buf;
				ang = *uint_ptr;
				uint_ptr++;
				lum = *uint_ptr;
				
				#ifdef debug
					printf("Received Parameters:\nAngle: %d\nLuminosity: %d\n",ang,lum);
				#endif
				
				LCD_CS();
				LCD_Write("Making Analysis");
				LCD_Cursor_Pos(1,0);
				
				LCD_ident_num(ang,&string_aux[0]);
				LCD_Write("Ang: ");
				LCD_Write(string_aux);
				
				LCD_ident_num(lum,&string_aux[0]);
				LCD_Write(" Lum: ");
				LCD_Write(string_aux);
				_delay_ms(1000);
				
				analysis(&analysis_data,ang,lum);
				
				s1.socketSendData((uint8_t*)&analysis_data,DATA_PKG_SIZE);
				#ifdef debug
					printf("Sending %d bytes of data...\n", sizeof(analysis_pkg_t));
				#endif
				
				LCD_CS();
				LCD_Write("Analysis Done");
				LCD_Cursor_Pos(1,0);
				LCD_Write("Disconnecting..");
				_delay_ms(1000);
				
				s1.socketDisconnect();
			}
				
		}
		
		if(s1.sockStat() == SOCK_CLOSE_WAIT)
		{
			s1.socketClose();
		}
		
		if(s1.sockStat() == SOCK_CLOSED)
		{
			s1.socketOpen();
			s1.socketListen();
			
			enable_LCD_refresh();
		}
		
		if(USART_Available_Bytes() >= sizeof(network_parameters_t)) // if user update network parameters...
		{
			printf("Data Received!!!\n");
			if(networkConf())
				goto reconfig; // go to the W5100 initialization
		}
				
	}// loop 
			
}// end main


int networkConf()
{		
	unsigned int x;
	network_parameters_t param;
	unsigned char * d;
	
	d = (unsigned char *)&param;	
	
	// read data from USART ring buffer
	for(x = 0; x < sizeof(network_parameters_t) ; x++)
	{
		if(!USART_Read_Byte(d))
			return 0;
		d++;
	}
	
	printf("Storing new parameters...\n");
	
	W5100_Store_NetParam(&param);
	
	return 1;
}



void simulation(analysis_pkg_t* pkg, uint16_t ang, uint16_t lum)
{	
	#ifdef debug
		printf("Simulation Requested\n");
	#endif
	
	PWM_ServoPos(ang);
	DIMMER_Intensity(lum);
	_delay_ms(500);
	
	pkg->vcoef = COEF_V;
	pkg->icoef = COEF_I;
	pkg->tcoef = COEF_T;
	
	pkg->vp[0] = 20; 
	pkg->ip[0] = 1023;
	
	int x;
	for(x=1;x<np;x++)
	{
		pkg->vp[x] = pkg->vp[x-1] + 10;
		pkg->ip[x] = pkg->ip[x-1] - 10;
	}
	
	pkg->tp = 512;
	
	#ifdef debug
		printf("Simulation Done\n");
	#endif
	
	PWM_ServoPos(90);
	DIMMER_Intensity(0);

	
	return;
}

void analysis(analysis_pkg_t* pkg, uint16_t ang, uint16_t lum)
{		
	unsigned int vcount = 0;
	unsigned int vp_med;
	unsigned int ip_med;
	unsigned int tp = ADC_ReadTp();
	
	// -> Adjust the system with analisys requiriments
	PWM_ServoPos(ang);
	DIMMER_Enable();
	DIMMER_Intensity(lum);
	_delay_ms(4000); // delay to adjustment
	
	// -> Read the Vp open circuit value
	// -> Calculate the voltage step for the given number of points
	unsigned int step = (ADC_ReadVp()*40.)/np; //  saves only the integer part...
	
	int x;
	int y;
	
	PWM_SetCount(0);
	_delay_ms(6000); // -> delay to change voltage to max to minimum
	
	for(x=0;x<np;x++)
	{
		
		vp_med = 0;
		ip_med = 0;
		for(y=0;y<40;y++)
		{
			vp_med+=ADC_ReadVp();
			ip_med+=ADC_ReadIp();
		}
		
		pkg->vp[x] = (vp_med/40);
		pkg->ip[x] = (ip_med/40);
		
		
		vcount+=step;
		PWM_SetCount(vcount);
		_delay_ms(300); // need to test the system speed response -> best reponse = 300ms
	}
	
	tp+=ADC_ReadTp();
	pkg->tp = tp/2;
	
	PWM_SetCount(TOP-1);
	PWM_ServoPos(90);
	DIMMER_Disable();
}

void server_param_string(char * port_str, char * ip_str)
{
	
	// Convert port number to Ascii
	LCD_ident_num(W5100_Port_Number(),port_str);
	
	// Convert Ip number to Ascii
	unsigned char ip[4];
	W5100_Ip_Number(&ip[0]);

	
	char ip_part[6];
	int x = 0;
	int y;

	
	while (x<4)
	{
		LCD_ident_num((unsigned int) ip[x],&ip_part[0]);
		
		y=0;
		while(ip_part[y] != '\0')
		{
			*ip_str = ip_part[y];
			ip_str++;
			y++;
		}
		
		if(x==3)
		{
			*ip_str = '\0';
			break;
		}
		
		*ip_str = '.';
		ip_str++;
		x++;
	}
		
}


void refresh_LCD_init(void)
{
	TCCR0B = 0x00;        //Disable Timer0
	TIFR0  = 0x00;        //Clean interrupts flags
	TIMSK0 = 0x01;        //Enable Overflow Interruption
	TCCR0A = 0x00;        //No wave generation mode
}

void enable_LCD_refresh(void)
{
	TCNT0 = 0;
	TCCR0B = 0x05; // start timer with prescaler 1024
}

void disable_LCD_refresh(void)
{
	TCCR0B = 0x00; // disable timer clock
}

void refresh_LCD(int code)
{
	switch(code)
	{
		case 0:
		LCD_CS();
		LCD_Write("  Server Port:  ");
		LCD_Cursor_Pos(1,5);
		LCD_Write(string_port);
		break;
		
		case 1:
		LCD_CS();
		LCD_Write("  Server Ip:  ");
		LCD_Cursor_Pos(1,0);
		LCD_Write(string_ip);
		break;	
	}
	
}

ISR(TIMER0_OVF_vect)
{	
	static unsigned int cont = 0;
	static int x = 0;
	
	if(cont>350)
	{
		refresh_LCD(x);
		x++;
		if(x>MAX_LCD_CODES-1)
			x=0;
			
		cont = 0;
	}
	
	cont++;
	
}

