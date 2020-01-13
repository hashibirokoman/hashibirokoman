/*
atmega88 SPI communication
2019/09/30
*/

#include <avr/io.h>		/* Device include file */

void spi_master_init(unsigned char div);

uint8_t spi_send( uint8_t byte );
uint8_t spi_recv( void );