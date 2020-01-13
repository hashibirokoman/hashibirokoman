/*
 * usart_atmega88.c
 *
 * Created: 2019/09/30 0:43:59
 *  Author: yoda
 */ 
#include <avr/io.h>

// USART
#define BAUD 38400 /* USART baud rate */
#define MYUBRR F_CPU/16/BAUD-1 /* initialize num */

void USART_init(uint16_t baud);

void USART_text_out( char* text );
void USART_num_out( int16_t num, int8_t digit );
void USART_hex_out( unsigned char byte );

void USART_send(uint8_t data);
uint8_t USART_recv(void);

unsigned char dec2hexcode( unsigned char dec );
