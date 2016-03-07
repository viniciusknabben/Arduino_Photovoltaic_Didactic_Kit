/*
 * RING_BUFFER.cpp
 *
 * Created: 01/04/2015 13:06:17
 *  Author: Vinícius
 */ 

#include "RING_BUFFER.h"

RingBuffer * create_RB(void)
{
	RingBuffer * tmp = (RingBuffer*)malloc(sizeof(RingBuffer));
	tmp->wr_idx = 0;
	tmp->rd_idx = 0;
	tmp->size = 0;
	return tmp;
}

void destroy_RB(RingBuffer * f)
{
	free(f);
}

int rb_full(RingBuffer * f)
{
	return (f->size == MAXSIZE);
}

int rb_empty(RingBuffer * f)
{
	return (f->size == 0);
}

unsigned int rb_size(RingBuffer * f)
{
	return f->size;
}

int rb_put(RingBuffer * f, uchar8 d)
{
	if(rb_full(f))
		return 0;
		
	f->buffer[f->wr_idx++]=d;
	f->wr_idx %= MAXSIZE;
	f->size++;
	return 1;
}

uchar8 rb_remove(RingBuffer * f)
{
	if(rb_empty(f))
		return 0;
		
	uchar8 tmp = f->buffer[f->rd_idx++];
	f->rd_idx %= MAXSIZE;
	f->size--;
	return tmp;
}

void print_rb(RingBuffer * f)
{
	for(int i = 0; i < MAXSIZE; i++)
	printf("%3d ", (unsigned int)f->buffer[i]);
	printf("\n");
	
	for(int i = 0; i < (f->rd_idx*4+2); i++)
	printf(" ");
	printf("r\n");
	
	for(int i = 0; i < (f->wr_idx*4+2); i++)
	printf(" ");
	printf("w\n");
}