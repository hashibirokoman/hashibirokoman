/*
 * I2C_atmega88.c
 *
 * Created: 2019/10/07 22:41:40
 *  Author: yoda
 */ 
#include "I2C_atmega88.h"


char i2c_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // start send
	while (!(TWCR & (1<<TWINT))); // wait
	return (TWSR&0xF8); // state
}


char i2c_send(unsigned char send_data)
{
	TWDR = send_data; // send data
	TWCR = (1<<TWINT)|(1<<TWEN); //send
	while (!(TWCR & (1<<TWINT))); // wait
	return (TWSR&0xF8); // state
}


char i2c_recv_ack(unsigned char* read_data)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); // receive
	while (!(TWCR & (1<<TWINT))); // wait
	*read_data = TWDR;
	return (TWSR&0xF8); // state
}


char i2c_recv_nack(unsigned char* read_data)
{
	TWCR = (1<<TWINT)|(1<<TWEN); // receive
	while (!(TWCR & (1<<TWINT))); // wait
	*read_data = TWDR;
	return (TWSR&0xF8); // state
}


void i2c_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); // stop send
}


char i2c_data_write(unsigned short address, unsigned char write_data)
{
	char ret;
	unsigned char ad_H, ad_L;
	
	ret = i2c_start();
	if( ret != START )return ERR_ST; // state check
	
	ret = i2c_send(SLA_W);
	if( ret != SLA_W_ACK )return ERR_AD; // state check
	
	ad_H = (address&0xFF00)>>8;
	ret = i2c_send( ad_H );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ad_L = address&0x00FF;
	ret = i2c_send( ad_L );
	if( ret != SLD_ACK )return ERR_AD; // state check

	ret = i2c_send( write_data );
	if( ret != SLD_ACK )return ERR_DAT; // state check

	i2c_stop();
	
	return OK;
}

char i2c_data_read(unsigned short address, unsigned char* read_data)
{
	char ret;
	unsigned char ad_H, ad_L;
	
	ret = i2c_start();
	if( ret != START )return ERR_ST; // state check

	ret = i2c_send(SLA_W);
	if( ret != SLA_W_ACK )return ERR_AD; // state check
	
	ad_H = (address&0xFF00)>>8;
	ret = i2c_send( ad_H );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ad_L = address&0x00FF;
	ret = i2c_send( ad_L );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ret = i2c_start();
	if( ret != RESTART )return ERR_ST; // state check
	
	ret = i2c_send(SLA_R);
	if( ret != SLA_R_ACK )return ERR_AD; // state check

	ret = i2c_recv_nack(read_data);
	if( ret != MTD_NAK )return ERR_NAK; // state check
	
	i2c_stop();

	return OK;
}


char i2c_page_write(unsigned short page_add, unsigned char* write_data)
{
	char ret;
	unsigned char ad_H, ad_L;
	unsigned short i;
	
	ret = i2c_start();
	if( ret != START )return ERR_ST; // state check
	
	ret = i2c_send(SLA_W);
	if( ret != SLA_W_ACK )return ERR_AD; // state check
	
	ad_H = (page_add&0xFF00)>>8;
	ret = i2c_send( ad_H );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ad_L = page_add&0x00FF;
	ret = i2c_send( ad_L );
	if( ret != SLD_ACK )return ERR_AD; // state check

	for(i=0; i<PAGE_BYTE; i++)
	{
		ret = i2c_send( write_data[i] );
		if( ret != SLD_ACK )return ERR_DAT; // state check	
	}

	i2c_stop();
	
	return OK;
}


char i2c_page_read(unsigned short page_add, unsigned char* read_data)
{
	char ret;
	unsigned char ad_H, ad_L;
	unsigned short i;
	
	ret = i2c_start();
	if( ret != START )return ERR_ST; // state check

	ret = i2c_send(SLA_W);
	if( ret != SLA_W_ACK )return ERR_AD; // state check
	
	ad_H = (page_add&0xFF00)>>8;
	ret = i2c_send( ad_H );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ad_L = page_add&0x00FF;
	ret = i2c_send( ad_L );
	if( ret != SLD_ACK )return ERR_AD; // state check
	
	ret = i2c_start();
	if( ret != RESTART )return ERR_ST; // state check
	
	ret = i2c_send(SLA_R);
	if( ret != SLA_R_ACK )return ERR_AD; // state check

	for(i=0; i<(PAGE_BYTE-1); i++)
	{
		ret = i2c_recv_ack( read_data+i );
		if( ret != MTD_ACK )return ERR_ACK; // state check
	}
	
	ret = i2c_recv_nack(read_data+i);
	if( ret != MTD_NAK )return ERR_NAK; // state check

	i2c_stop();

	return OK;
}