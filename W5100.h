/*
 * w5100.h
 *
 * Created: 20/10/2014 22:48:36
 *  Author: Vinicius Knabben
 */
#ifndef W5100_H_INCLUDED
#define W5100_H_INCLUDED

#include "MAIN_DEFINITIONS.h"
#include <avr/eeprom.h>
#include "USART.h"


//SPI I/O UNO328P
#define SPI_PORT PORTB
#define SPI_DDR  DDRB

#define SPI_SCK  PB5 // pin 13
#define SPI_MISO PB4 // pin 11
#define SPI_MOSI PB3 // pin 12
#define SPI_SS   PB2 // pin 10


//SPI I/O MEGA2560 ---> We must set SS hardware pin as output even when we dont use it. if not, SPI doesn't work correctly
// #define SPI_PORT PORTB
// #define SPI_DDR  DDRB
// #define SPI_MISO PB3 // pin 50
// #define SPI_MOSI PB2 // pin 51
// #define SPI_SCK  PB1 // pin 52
// #define SPI_HSS	 PB0 // pin 50 -> Hardware SS
// #define SPI_SS	 PB4 // pin 10 -> Digital SS

// Wiznet W5100 Op Code
#define WIZNET_WRITE_OPCODE 0xF0
#define WIZNET_READ_OPCODE 0x0F

// Wiznet W5100 Ethernet Setup Register Addresses ------------- 
#define MR		0x0000   // Mode Register
#define GAR		0x0001   // Gateway Address: 0x0001 to 0x0004
#define SUBR	0x0005   // Subnet mask Address: 0x0005 to 0x0008
#define SAR		0x0009   // Source Hardware Address (MAC): 0x0009 to 0x000E
#define SIPR	0x000F   // Source IP Address: 0x000F to 0x0012
#define RMSR	0x001A   // RX Memory Size Register
#define TMSR	0x001B   // TX Memory Size Register

//----------------------------------------------

// Sn_MR values (Mode Register) -> SPI_Write(Sn_MR,command)
#define MR_CLOSE	0x00    // Unused socket
#define MR_TCP		0x01    // TCP
#define MR_UDP		0x02    // UDP
#define MR_IPRAW	0x03	  // IP LAYER RAW SOCK
#define MR_MACRAW	0x04	  // MAC LAYER RAW SOCK
#define MR_PPPOE	0x05	  // PPPoE
#define MR_ND		0x20	  // No Delayed Ack(TCP) flag
#define MR_MULTI	0x80	  // support multi casting

#define MEM_SPLIT_1	0x55
//#define MEM_SPLIT_2 0X65

#define Sn_SR		0x0003      // Socket n: Status Register Address
#define S_SR(msk)	(Sn_SR|msk)

//----------------------------------------------

//SPI Functions---------------------------------
void SPI_Init(void);
void SPI_Write(unsigned int addr,unsigned char data);
unsigned char SPI_Read(unsigned int addr);

//----------------------------------------------

//W5100 Functions-------------------------------

// Receive and store network parameters on EEPROM memory 
void W5100_Store_NetParam(network_parameters_t * net_param); 

// Return network parameters stored data on EEPROM memory
void W5100_Read_NetParam(network_parameters_t * net_param);

// Return server Ip
void W5100_Ip_Number(unsigned char * ip_value);

// Return the Port number stored in EEPROM memory
uint16_t W5100_Port_Number(void);

// initiate Registers on W5100 //
int W5100_Init(void);

//----------------------------------------------

#endif // W5100_H_INCLUDED
