/*
 * I2C_atmega88.h
 *
 * Created: 2019/10/07 22:42:43
 *  Author: yoda
 */ 
#ifndef I2C_ATMEGA88_H_
#define I2C_ATMEGA88_H_

#include <avr/io.h>		/* Device include file */
// slave address
#define SLA_W 0xA2
#define SLA_R 0xA3

#define	START	0x08
#define RESTART 0x10
#define	RECEIVE	0x60
#define	SLA_W_ACK 0x18
#define	SLD_ACK	0x28
#define SLA_R_ACK 0x40
#define MTD_ACK 0x50
#define MTD_NAK 0x58

#define OK 0
#define ERR 1
#define ERR_ACK 2
#define ERR_NAK 3
#define ERR_ST 4
#define ERR_AD 5
#define ERR_DAT 6

#define PAGE_BYTE 128

char i2c_start(void);
char i2c_send(unsigned char send_data);
char i2c_recv_ack(unsigned char* read_data);
char i2c_recv_nack(unsigned char* read_data);
void i2c_stop(void);

char i2c_data_write(unsigned short address, unsigned char write_data);
char i2c_data_read(unsigned short address, unsigned char* read_data);

char i2c_page_write(unsigned short page_add, unsigned char* write_data);
char i2c_page_read(unsigned short page_add, unsigned char* read_data);

#endif /* I2C_ATMEGA88_H_ */