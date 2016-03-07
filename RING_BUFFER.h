/*
 * RING_BUFFER.h
 *
 * Created: 01/04/2015 13:06:01
 *  Author: Vinícius
 */ 


#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uchar8;

#define MAXSIZE 50
typedef struct 
{
	uchar8 buffer[MAXSIZE];
	int rd_idx;
	int wr_idx;
	unsigned int size;
	
}RingBuffer;

/* alloc memory for the ring buffer and return ist address */
RingBuffer * create_RB(void);

/* Destroy the RB by his address */
void destroy_RB(RingBuffer * f);

/* return number of bytes on RB */
unsigned int rb_size(RingBuffer * f);

/* Add byte to the RB and returns true(1) or false(0) */
int rb_put(RingBuffer * f, uchar8 d);

/* Remove byte from RB and return it */
uchar8 rb_remove(RingBuffer * f);
	
/* Print buffer data */
void print_rb(RingBuffer * f);
	
/* Full or Empty verifiers */
int rb_full(RingBuffer * f); 
int rb_empty(RingBuffer * f);

#endif /* RING_BUFFER_H_ */