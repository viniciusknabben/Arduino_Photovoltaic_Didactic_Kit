/*
 * SOCKET_W5100.cpp
 *
 * Created: 26/02/2015 09:49:57
 *  Author: Vinícius
 */ 

#include "SOCKET_W5100.h"

int W5100_SocketsAvailable(void)
{
	int sockets_available = 0;
	uint16_t msk = SR_BA;
	
	for(int sock=0;sock<MAX_SOCKETS;sock++) // verifies if there is sockets available
	{
		if(!SPI_Read(S_SR(msk))) // verify socket status -> 0x00 means Closed -> Ready to open
		sockets_available++;
		
		msk+=SR_BA_OFF; // Point the mask to the next socket
	}
	
	return sockets_available; // return the number of sockets available
}

Socket_W5100::Socket_W5100(void)
{
	
	/*
		Detect sockets available and assign the object to one socket
		
		if there is no socket available return error msg.	
	*/	
	
	int sockA;
	uint16_t msk = 0x0400;
	
	server_port = 80;
	
	for(sockA = 0;sockA<4;sockA++) // verifies if there is sockets available
	{
		if(!SPI_Read(S_SR(msk))) // verify socket status -> assign the object to the first available socket -> 00 means closed(available)
			break;
			
		msk+=0x0100; // Point the mask to the next socket
	}
	
	switch (sockA)
	{
		case 0: // mask offset for socket 0
			sn = 0;
			mask = 0x0400;
		break;
		
		case 1: // mask offset for socket 1
			sn = 1;
			mask = 0x0500;
		break;
		
		case 2: // mask offset for socket 2
			sn = 2;
			mask = 0x0600;
		break;
		
		case 3: // mask offset for socket 3
			sn = 3;
			mask = 0x0700;
		break;
		
		default:
			//error msg
		break;
	}
	
}



void Socket_W5100::socketClose(void)
{
	SPI_Write(S_CR(mask),CR_CLOSE); // Send command to close the socket  
	while(SPI_Read(S_SR(mask))); // waits until the socket is closed -> Sn_SR != SOCK_CLOSED ---> SOCK_CLOSED = 0x00
	
	#ifdef debug
		printf("Socket Closed\n");
	#endif
}

void Socket_W5100::socketDisconnect(void)
{
	SPI_Write(S_CR(mask),CR_DISCON); // Send Disconect request to the client
	while(SPI_Read(S_SR(mask)));
	
	#ifdef debug
		printf("Socket send disconnect request\n");
	#endif
	
}

void Socket_W5100::setPort(uint16_t p)
{
	server_port = p;
}

uint8_t Socket_W5100::socketOpen(void) // Datasheet page 41
{
	
	if (SPI_Read(S_SR(mask))) //Closes the socket if it's already open
	{
		socketClose();
	}
	
	/* Set Socket Mode (TCP) */
	SPI_Write(S_MR(mask),MR_TCP); // Send TCP mode command

	/* Set Server Port */
	SPI_Write(S_PORT(mask),((server_port & 0xFF00) >> 8 ));	// Send socket port  high
	SPI_Write(S_PORT(mask) + 1,(server_port & 0x00FF));		// Send socket port  low
		
	/* Open Socket Mode */
	SPI_Write(S_CR(mask),CR_OPEN); // Send Open command
	while(SPI_Read(S_CR(mask))); // Waits CR perform the command (waits Sn_CR == 0x00)

	/* Verify if the socket is Open */
	if (SPI_Read(S_SR(mask)) != SOCK_INIT)
	{
		#ifdef debug
			printf("Error opening socket\n");
		#endif
		return 0;
	}	
	else
	{
		#ifdef debug
			printf("Server Opened on Port: %u\n", server_port);
		#endif
		return 1;
	}	
}

uint8_t Socket_W5100::socketListen(void) // Datasheet page 42
{
	
	if (SPI_Read(S_SR(mask)) == SOCK_INIT)
	{
		SPI_Write(S_CR(mask),CR_LISTEN);

		while(SPI_Read(S_CR(mask)));
		
		/*verify socket state*/
		if (SPI_Read(S_SR(mask)) != SOCK_LISTEN)
		{
			#ifdef debug
				printf("Error on opening server listening, closing socket\n");
			#endif
			socketClose();
			return 0;
		}		
		else
		{	
			#ifdef debug
				printf("Server Listening\n");
			#endif
		}		
	}
	return 1;
}

uint8_t Socket_W5100::socketStatus(void)
{
	
	#ifdef debug
	
		char socket_status = SPI_Read(S_SR(mask));
		
		switch (socket_status)
		{
			case SOCK_CLOSED:
				printf("Socket Closed\n");
			return SOCK_CLOSED;
			
			case SOCK_INIT:
				printf("Socket Initializing\n");
			return SOCK_INIT;
			
			case SOCK_LISTEN:
				printf("Socket Listening\n");
			return SOCK_LISTEN;
			
			case SOCK_SYNSENT:
				printf("Socket Connecting - SYN SENT\n");
			return SOCK_SYNSENT;
			
			case SOCK_SYNRECV:
				printf("Socket Connecting - SYN RECV\n");
			return SOCK_SYNRECV;
			
			case SOCK_ESTABLISHED:
				printf("Socket Connected\n");
			return SOCK_ESTABLISHED;
			
			case SOCK_FIN_WAIT:
				printf("Socket Closing - FIN WAIT\n");
			return SOCK_FIN_WAIT;
			
			case SOCK_TIME_WAIT:
				printf("Socket Closing - TIME WAIT\n");
			return SOCK_TIME_WAIT;
			
			case SOCK_CLOSE_WAIT:
				printf("Socket Closing - CLOSE WAIT\n");
			return SOCK_CLOSE_WAIT;
			
			case SOCK_LAST_ACK:
				printf("Socket Closing - LAST ACK\n");
			return SOCK_LAST_ACK;
			
			default:
				printf("Unknown Error\n");
			return 0xFF;
			
		}
	#endif
	
	
	return SPI_Read(S_SR(mask));	
}

uint8_t Socket_W5100::sockStat(void)
{
	return SPI_Read(S_SR(mask));
}

uint16_t Socket_W5100::socketSendData(uint8_t *buf,uint16_t send_size) // Datasheet page 33 and 44
{
	uint16_t ptr,offaddr,realaddr,txsize,timeout;
	
	if(send_size <= 0)
		return 0;

	/* Check the TX buffer free Size */
	txsize=SPI_Read(S_TX_FSR(mask)); // read the free size of tx buffer -> high register
	txsize=(((txsize & 0x00FF) << 8 ) + SPI_Read(S_TX_FSR(mask) + 1)); // read the free size of tx buffer -> low register


	timeout=0;
	while (txsize < send_size) // waits for 1s for free memory
	{
		_delay_ms(1);

		txsize=SPI_Read(S_TX_FSR(mask));
		txsize=(((txsize & 0x00FF) << 8 ) + SPI_Read(S_TX_FSR(mask) + 1));

		if (timeout++ > 1000)
		{
			socketDisconnect();
			return 0;
		}
	}
	

	// Read the Tx Write Pointer actual position
	ptr = SPI_Read(S_TX_WR(mask)); // Read high register
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S_TX_WR(mask) + 1)); // read low register

	while(send_size) // While data to transfer...
	{
		send_size--;
	
		// Calculate the physical address
		realaddr = S_TX_BA(sn) + (offaddr & TX_BUF_MASK); 
		
		// Copy the application data to the W5100 Tx Buffer
		SPI_Write(realaddr,*buf);
		offaddr++;
		buf++;
	}

	// Update the Sn_TX_WR value, so it point to the next transmit
	SPI_Write(S_TX_WR(mask),(offaddr & 0xFF00) >> 8 );
	SPI_Write(S_TX_WR(mask) + 1,(offaddr & 0x00FF));

	// Now Send the SEND command
	SPI_Write(S_CR(mask),CR_SEND);

	// Wait for Sending Process
	while(SPI_Read(S_CR(mask)));

	return 1;
}

uint16_t Socket_W5100::socketReceiveData(uint8_t *buf,uint16_t receive_size)
{
	uint16_t ptr,offaddr,realaddr;

	if (receive_size <= 0)
		return 0;

	// If the request size > MAX_BUF, truncate it
	if (receive_size > MAX_BUF)
		receive_size = MAX_BUF - 1;

	// Read the Rx Read Pointer
	ptr = SPI_Read(S_RX_RD(mask));
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S_RX_RD(mask) + 1));

	while(receive_size)
	{
		receive_size--;
		realaddr = RXBUFADDR + (offaddr & RX_BUF_MASK); //  base address + (offset & mask) -> mask = 0x07FF = 00000111 11111111 = 2047 adresses -> 2kb
		*buf = SPI_Read(realaddr);
		offaddr++;
		buf++;
	}
	
	*buf = '\0'; // put null to an string 
	
	// Update the S0_RX_RD value, so it point to the next receive
	SPI_Write(S_RX_RD(mask),(offaddr & 0xFF00) >> 8 );
	SPI_Write(S_RX_RD(mask) + 1,(offaddr & 0x00FF));

	// Now Send the RECV command
	SPI_Write(S_CR(mask),CR_RECV);
	_delay_us(5);    // Wait for Receive Process

	return 1;
}

uint16_t Socket_W5100::receiveSize(void)
{
	return ((SPI_Read(S_RX_RSR(mask)) & 0x00FF) << 8 ) + SPI_Read(S_RX_RSR(mask) + 1);
}

