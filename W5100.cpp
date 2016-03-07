/*
 * w5100.cpp
 *
 * Created: 20/10/2014 22:49:15
 *  Author: Vinicius Knabben
 */

#include "W5100.h"

network_parameters_t EEMEM stored_np;
unsigned char EEMEM mem_flag;

void SPI_Init(void)
{
    // Setup I/O
	SPI_DDR |= (1<<SPI_SCK)|(1<<SPI_MOSI)|(1<<SPI_SS);// add when using mega |(1<<SPI_HSS);

    // set SS as high 
    set_bit(SPI_PORT,SPI_SS);

    // Setup SPI
    /*
        Mode:           Master mode.
        Polarity:       SCK Low when idle. 
        Phase:          Falling
        Clock:          F_CPU/4
        Double Speed:   Enabled
		
		Final Speed:	F_CPU/2 = 8MHz
		
		Slave Select:	High -> Stop comunications wiht W5100
						Low  -> Start comunications with W5100
						
		MISO pin is override input

    */
    SPCR = (1<<SPE)|(1<<MSTR);
    SPSR = (1<<SPI2X);
	
}	

void SPI_Write(unsigned int addr,uint8_t data)
{
    clr_bit(SPI_PORT,SPI_SS); // start SPI setting slave pin low

    SPDR = WIZNET_WRITE_OPCODE; // send write opcode to w5100
    while(!tst_bit(SPSR,SPIF)); 

    SPDR = (addr & 0xFF00) >> 8; // send high address value
    while(!tst_bit(SPSR,SPIF));

    SPDR = addr & 0x00FF; // send low address value
    while(!tst_bit(SPSR,SPIF));

    SPDR = data;
    while(!tst_bit(SPSR,SPIF));

    set_bit(SPI_PORT,SPI_SS); // stop SPI setting slave pin high
}

unsigned char SPI_Read(uint16_t addr)
{
    clr_bit(SPI_PORT,SPI_SS);

    SPDR = WIZNET_READ_OPCODE; // send read opcode to w5100
    while(!tst_bit(SPSR,SPIF));

    SPDR = (addr & 0xFF00) >> 8;
    while(!tst_bit(SPSR,SPIF));

    SPDR = addr & 0x00FF;
    while(!tst_bit(SPSR,SPIF));

    SPDR = 0x00; // Send Dummy transmission for reading the data
    while(!tst_bit(SPSR,SPIF));

    set_bit(SPI_PORT,SPI_SS);

    return(SPDR);
}

void W5100_Store_NetParam(network_parameters_t * net_param)
{
	eeprom_write_block(net_param,&stored_np,sizeof(network_parameters_t));
	eeprom_write_byte(&mem_flag,0xFF);
}

void W5100_Read_NetParam(network_parameters_t * net_param)
{	
	eeprom_read_block(net_param,&stored_np,sizeof(network_parameters_t));	
}

uint16_t W5100_Port_Number(void)
{
	network_parameters_t net_param;
	W5100_Read_NetParam(&net_param);
	
	return net_param.port;
}

void W5100_Ip_Number(unsigned char * ip_value)
{
	network_parameters_t net_param;
	W5100_Read_NetParam(&net_param);
	
	*ip_value = net_param.ip_addr[0];
	ip_value++;
	*ip_value = net_param.ip_addr[1];
	ip_value++;
	*ip_value = net_param.ip_addr[2];
	ip_value++;
	*ip_value = net_param.ip_addr[3];
}


int W5100_Init(void)
{
	SPI_Init();
	
	network_parameters_t net_param;
	W5100_Read_NetParam(&net_param);
	
	if(eeprom_read_byte(&mem_flag))
	{
		#ifdef debug
			printf("Initializing W5100 Network Parameters\n\n");
		#endif
	}
	else
	{
		#ifdef debug
			printf("No Network Parameters on Memory\n\n");
		#endif	
		return 0;
	}			
				
	// Setting the Wiznet W5100 Mode Register: 0x0000
	SPI_Write(MR,0x80);            // MR = 0b10000000;
	_delay_ms(1);
	
	// Setting the Wiznet W5100 Gateway Address Register (GAR): 0x0001 to 0x0004
	SPI_Write(GAR + 0,net_param.gtw_addr[0]);
	SPI_Write(GAR + 1,net_param.gtw_addr[1]);
	SPI_Write(GAR + 2,net_param.gtw_addr[2]);
	SPI_Write(GAR + 3,net_param.gtw_addr[3]);
	_delay_ms(1);
	
	#ifdef debug
		 printf("Gateway Value: %d.%d.%d.%d\n",SPI_Read(GAR),SPI_Read(GAR+1),\
		 SPI_Read(GAR+2), SPI_Read(GAR+3));
	#endif
		
	// Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
	SPI_Write(SAR + 0,net_param.mac_addr[0]);
	SPI_Write(SAR + 1,net_param.mac_addr[1]);
	SPI_Write(SAR + 2,net_param.mac_addr[2]);
	SPI_Write(SAR + 3,net_param.mac_addr[3]);
	SPI_Write(SAR + 4,net_param.mac_addr[4]);
	SPI_Write(SAR + 5,net_param.mac_addr[5]);
	_delay_ms(1);
	
	#ifdef debug
		printf("MAC Value: %x-%x-%x-%x-%x-%x\n",SPI_Read(SAR),SPI_Read(SAR+1),\
		SPI_Read(SAR+2), SPI_Read(SAR+3), SPI_Read(SAR+4), SPI_Read(SAR+5));
	#endif
		
	// Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
	SPI_Write(SUBR + 0,net_param.sub_mask[0]);
	SPI_Write(SUBR + 1,net_param.sub_mask[1]);
	SPI_Write(SUBR + 2,net_param.sub_mask[2]);
	SPI_Write(SUBR + 3,net_param.sub_mask[3]);
	_delay_ms(1);
	
	#ifdef debug
		printf("Sub Mask Value: %d.%d.%d.%d\n",SPI_Read(SUBR),SPI_Read(SUBR+1),\
		SPI_Read(SUBR+2), SPI_Read(SUBR+3));
	#endif
	
	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	SPI_Write(SIPR + 0,net_param.ip_addr[0]);
	SPI_Write(SIPR + 1,net_param.ip_addr[1]);
	SPI_Write(SIPR + 2,net_param.ip_addr[2]);
	SPI_Write(SIPR + 3,net_param.ip_addr[3]);
	_delay_ms(1);
	
	#ifdef debug
		printf("Ip Value: %d.%d.%d.%d\n\n",SPI_Read(SIPR),SPI_Read(SIPR+1),\
		SPI_Read(SIPR+2), SPI_Read(SIPR+3));
	#endif
	
	// Setting the Wiznet W5100 RX and TX Memory 2KB for each of Rx/Tx 4 channels
	SPI_Write(RMSR,MEM_SPLIT_1);
	SPI_Write(TMSR,MEM_SPLIT_1);
	
	#ifdef debug
		printf("Done\n\n");
	#endif
	
	return 1;

}

