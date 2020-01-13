/*
 * spi_atmega328.c
 *
 * Created: 2019/09/30 23:08:32
 *  Author: yoda
 */ 

#include "spi_atmega88.h"

/* SPI initialize functions */
// div = SPI clock div (2,4,8,16,32,64,128)
// default:SD card initialize clock
void spi_master_init(unsigned char div)
{
	switch(div)
	{
		case 2:
			SPSR = (1 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1);
		break;
		case 4:
			SPSR = (0 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1);
		break;
		case 8:
			SPSR = (1 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (0 << SPR1);
		break;
		case 16:
			SPSR = (0 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (0 << SPR1);
		break;
		case 32:
			SPSR = (1 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (1 << SPR1);
		break;
		case 64:
			SPSR = (0 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (1 << SPR1);
		break;
		case 128:
			SPSR = (0 << SPI2X);
			SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
		break;
	}

}

/* SPI send */
uint8_t spi_send( uint8_t byte )
{
	SPDR = byte;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

/* SPI receive */
uint8_t spi_recv(void)
{
	SPDR = 0xff;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}