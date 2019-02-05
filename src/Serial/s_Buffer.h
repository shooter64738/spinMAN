/*
 * s_Buffer.h
 *
 * Created: 1/10/2018 10:36:36 PM
 *  Author: jeff_d
 */ 


#ifndef S_BUFFER_H_
#define S_BUFFER_H_
#define RX_BUFFER_SIZE 256

typedef struct
{
	char Buffer[RX_BUFFER_SIZE];
	int volatile Head;
	int volatile Tail;
	bool OverFlow;
	int volatile EOL;

} s_Buffer;



#endif /* S_BUFFER_H_ */