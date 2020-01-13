/*
 * usart_atmega88.c
 *
 * Created: 2019/09/30 0:43:59
 *  Author: yoda
 */ 
#include "usart_atmega88.h"

/* USART initialize */
void USART_init(uint16_t baud)
{
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t)baud;
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

void USART_send(uint8_t data)
{
	while( !(UCSR0A&(1<<UDRE0)) );
	UDR0 = data;
}

uint8_t USART_recv(void)
{
	while( !(UCSR0A&(1<<RXC0)) );
	return UDR0;
}

// UART out function
void USART_text_out( char* text )
{
	uint8_t  i = 0;

	while('\0' != *(text+i))
	{
		USART_send((char)(*(text+i)));
		i++;
	}
}


void USART_num_out( int16_t num, int8_t digit )
{
	int8_t msg_num[5],i;

	if( num < 0 )
	{
		USART_send('-');
		num = -1*num;
	}

	if(digit>=5){ msg_num[4] = num / 10000; num = num % 10000; }
	if(digit>=4){ msg_num[3] = num / 1000 ; num = num % 1000; }
	if(digit>=3){ msg_num[2] = num / 100  ; num = num % 100; }
	if(digit>=2){ msg_num[1] = num / 10   ; num = num % 10; }
	if(digit>=1){ msg_num[0] = num ;}
	
	for (i = digit-1; i>=0; i--)USART_send(msg_num[i] + '0'); // data send loop
}

// byte decimal num -> hex text code
// byte:0-255
void USART_hex_out( unsigned char byte )
{
	unsigned char bit_H, bit_L;
	
	bit_H = dec2hexcode( byte/16 );
	bit_L = dec2hexcode( byte%16 );
	USART_send(bit_H);
	USART_send(bit_L);
	USART_send(' ');
}

// decimal num -> hex code (dec:0-15)
unsigned char dec2hexcode( unsigned char dec )
{
	unsigned char out;
	
	switch(dec)
	{
		case 0: out = '0'; break;
		case 1: out = '1'; break;
		case 2: out = '2'; break;
		case 3: out = '3'; break;
		case 4: out = '4'; break;
		case 5: out = '5'; break;
		case 6: out = '6'; break;
		case 7: out = '7'; break;
		case 8: out = '8'; break;
		case 9: out = '9'; break;
		case 10: out = 'A'; break;
		case 11: out = 'B'; break;
		case 12: out = 'C'; break;
		case 13: out = 'D'; break;
		case 14: out = 'E'; break;
		case 15: out = 'F'; break;
	}
	
	return out;
}