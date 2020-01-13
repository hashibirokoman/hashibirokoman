/*
 * Copyright (c) 2011-2014 NecoTech.org, All rights reserved.
 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "typedef.h"

#define XRESET	_BV(PORTD2)
#define DREQ	_BV(PORTB1)
#define XDCS	_BV(PORTB6)
#define XCS		_BV(PORTB7)

#define LED_VS	_BV(PORTB0)

#define SM_DIFF			0
#define SM_SETTOZERO	1
#define SM_RESET		2
#define SM_OUTOFWAV		3
#define SM_PDOWN		4
#define SM_TESTS		5
#define	SM_STREAM		6
#define SM_PLUSV		7
#define	SM_DACT			8
#define	SM_SDIORD		9
#define	SM_SDISHARE		10
#define	SM_SDINEW		11
#define	SM_ADPCM		12
#define	SM_ADPCM_HP		13

#define MC_MODE		0x0
#define	MC_BASS		0x2
#define MC_VOL		0xB
#define MC_CLOCKF	0x3
#define MC_AUDATA	0x5

#define XRESET_ON		(PORTD &= ~XRESET)
#define XRESET_OFF		(PORTD |= XRESET)
#define SELECT_XCS		(PORTB &= ~XCS)
#define UNSELECT_XCS	(PORTB |= XCS)
#define SELECT_XDCS 	(PORTB &= ~XDCS)
#define UNSELECT_XDCS 	(PORTB |= XDCS)

/*#define MP3_POWER_ON()	(PORTD |= PORTD3) */
/*#define MP3_POWER_OFF()	(PORTD &= ~PORTD3)*/

//#define VS1011E_SPI_SOFT
#define VS1011E_DATA_VALIDATE
#define VS1011E_SLOW_WAIT

void vs1011e_init();
void vs1011e_cmd(BYTE reg, DWORD cmd);
DWORD vs1011e_read(BYTE reg);

void set_volume();
void set_clock();
void set_bass();

void wait_for_ready();

void test_sin_start();
void test_sin_end();

void send_array(BYTE buf[], unsigned short len);
BYTE spi_byte_soft(BYTE data);
