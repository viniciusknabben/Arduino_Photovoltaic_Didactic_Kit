/*
 * SOCKET_W5100.h
 *
 * Created: 26/02/2015 09:49:16
 *  Author: Vinícius
 */ 


#ifndef SOCKET_W5100_H_
#define SOCKET_W5100_H_

#include "MAIN_DEFINITIONS.h"
#include "W5100.h"

// Socket register base address and offset
#define SR_BA	   0x0400
#define SR_BA_OFF  0x0100

// Socket 'n' Register Addresses 
#define Sn_MR	   0x0000      // Socket n: Mode Register Address
#define Sn_CR	   0x0001      // Socket n: Command Register Address
#define Sn_IR	   0x0002      // Socket n: Interrupt Register Address
#define Sn_SR	   0x0003      // Socket n: Status Register Address
#define Sn_PORT    0x0004      // Socket n: Source Port: 0x0N04 to 0x0N05
#define Sn_TX_FSR  0x0020      // Socket n: Tx Free Size Register: 0x0N20 to 0x0N21
#define Sn_TX_RD   0x0022      // Socket n: Tx Read Pointer Register: 0x0N22 to 0x0N23
#define Sn_TX_WR   0x0024      // Socket n: Tx Write Pointer Register: 0x0N24 to 0x0N25
#define Sn_RX_RSR  0x0026      // Socket n: Rx Received Size Pointer Register: 0x0N25 to 0x0N27
#define Sn_RX_RD   0x0028      // Socket n: Rx Read Pointer: 0x0428 to 0x0429

// Macro for Registers assign
#define S_MR(msk)		(Sn_MR|msk)
#define S_CR(msk)		(Sn_CR|msk)
#define S_IR(msk)		(Sn_IR|msk)
#define S_SR(msk)		(Sn_SR|msk)
#define S_PORT(msk)		(Sn_PORT|msk)
#define S_TX_FSR(msk)	(Sn_TX_FSR|msk)
#define S_TX_RD(msk)	(Sn_TX_RD|msk)
#define S_TX_WR(msk)	(Sn_TX_WR|msk)
#define S_RX_RSR(msk)	(Sn_RX_RSR|msk)
#define S_RX_RD(msk)	(Sn_RX_RD|msk)


// Sn_MR values (Mode Register) -> SPI_Write(Sn_MR,command)
#define MR_CLOSE	  0x00    // Socket Closed -> Unused socket
#define MR_TCP		  0x01    // TCP
#define MR_UDP		  0x02    // UDP
#define MR_IPRAW	  0x03	  // IP LAYER RAW SOCK
#define MR_MACRAW	  0x04	  // MAC LAYER RAW SOCK
#define MR_PPPOE	  0x05	  // PPPoE
#define MR_ND		  0x20	  // No Delayed Ack(TCP) flag
#define MR_MULTI	  0x80	  // support multicasting

// Sn_CR values	(Command Register values) -> SPI_Write(Sn_CR,command)  
// ----> After performing commands, the register value will be automatically cleared to 0x00 <------ //
#define CR_OPEN          0x01	  // Initialize or open socket
#define CR_LISTEN        0x02	  // Wait connection request in tcp mode(Server mode)
#define CR_CONNECT       0x04	  // Send connection request in tcp mode(Client mode)
#define CR_DISCON        0x08	  // Send closing reqeuset in tcp mode
#define CR_CLOSE         0x10	  // Close socket
#define CR_SEND          0x20	  // Update Tx memory pointer and send data
#define CR_SEND_MAC      0x21	  // Send data with MAC address, so without ARP process
#define CR_SEND_KEEP     0x22	  // Send keep alive message
#define CR_RECV          0x40	  // Update Rx memory buffer pointer and receive data

// Sn_SR values (Status Register values) -> SPI_Read(Sn_SR)
#define SOCK_CLOSED      0x00     // Closed
#define SOCK_INIT        0x13	  // Init state
#define SOCK_LISTEN      0x14	  // Listen state
#define SOCK_SYNSENT     0x15	  // Connection state
#define SOCK_SYNRECV     0x16	  // Connection state
#define SOCK_ESTABLISHED 0x17	  // Success to connect
#define SOCK_FIN_WAIT    0x18	  // Closing state
#define SOCK_CLOSING     0x1A	  // Closing state
#define SOCK_TIME_WAIT	 0x1B	  // Closing state
#define SOCK_CLOSE_WAIT  0x1C	  // Closing state
#define SOCK_LAST_ACK    0x1D	  // Closing state
#define SOCK_UDP         0x22	  // UDP socket
#define SOCK_IPRAW       0x32	  // IP raw mode socket
#define SOCK_MACRAW      0x42	  // MAC raw mode socket
#define SOCK_PPPOE       0x5F	  // PPPOE socket


// Rx/Tx W5100 Base Addresses

#define TXBUFADDR		0x4000   // W5100 Tx Base Address
#define RXBUFADDR		0x6000   // W5100 Rx Base Address

// Memory split 2Kb -> 0 to 0x07FF Addresses of memory
#define TX_BUF_MASK     0x07FF   // Tx 2K Buffer Mask:
#define RX_BUF_MASK     0x07FF   // Rx 2K Buffer Mask:  

// RX/TX Base Addresses of each socket
#define S_TX_BA(n)		TXBUFADDR+(0x0800*n) // Calculate the TX base address to the socket number 'n'
#define S_RX_BA(n)		RXBUFADDR+(0x0800*n) // Calculate the RX base address to the socket number 'n'

#define MAX_BUF			512
#define TCP_PORT        80       // TCP/IP Port

// forward Declaration for global Functions
#define MAX_SOCKETS 4

int W5100_SocketsAvailable(void);

class Socket_W5100
{
	public:
	
		Socket_W5100(void);
		
		void socketClose(void);
		void socketDisconnect(void);
		void setPort(uint16_t port_num);
		uint8_t socketOpen(void);
		uint8_t socketListen(void);
		uint8_t socketStatus(void);
		uint16_t socketSendData(uint8_t * buffer_ptr, uint16_t data_size);
		uint16_t socketReceiveData(uint8_t * buffer_ptr, uint16_t data_size);
		uint16_t receiveSize(void);
		
		
		uint8_t sockStat(void);
	
	private:
		
		uint16_t sn;			// Socket number
		uint16_t mask;			// Socket mask
		uint16_t server_port;	// Socket server port
		
	
};


#endif /* SOCKET_W5100_H_ */