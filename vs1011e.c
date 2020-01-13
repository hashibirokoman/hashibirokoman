/*
 * vs1011e.c
 *
 * Created: 2019/05/12
 * Author : yoda
 */ 

#include "spi_atmega88.h"
#include "usart_atmega88.h"
#include "vs1011e.h"

DWORD vols[] = { 0x5050, 0x3A3A, 0x1010 };
int currentVol = 1;

DWORD bass[] = { 0x7AF6, 0x44E6, 0x0 };
int curbass = 0;

void set_bass()
{
	vs1011e_cmd(MC_BASS, 0x0000);
	if (curbass > 2) curbass = 0;
	wait_for_ready();
}

void vs1011e_init()
{
	UNSELECT_XCS;
	UNSELECT_XDCS;

	// reset initialize
	XRESET_ON;  // hardware reset
	_delay_ms(1);
	XRESET_OFF;  // unlock reset
	_delay_ms(10);
	
	// mode setting
	vs1011e_cmd(MC_MODE, _BV(SM_TESTS)| _BV(SM_SDINEW) /*| _BV(SM_SDISHARE)*/);
	// clock setting
	vs1011e_cmd(MC_CLOCKF, 0x9770);
	// volume setting
	vs1011e_cmd(MC_VOL, 0x0000);
//	sampling rate
//	vs1011e_cmd(MC_AUDATA, 0xAC44);
	// bass setting
	vs1011e_cmd(MC_BASS, 0x0000);
}


void wait_for_ready()
{
	if (!(PINB & DREQ))
	{
		USART_text_out("DREQ no wait \n\r");
		return;
	}

	while (!(PINB & DREQ))
	{
		USART_text_out("DREQ wait...\n\r");
#ifdef VS1011E_SLOW_WAIT
		PORTB |= LED_VS;
		if ((PINB & DREQ))
		{
			for (int i = 0; i < 3; i++)
			{
				PORTB |= LED_VS;
				_delay_ms(5);
				PORTB &= ~LED_VS;
				_delay_ms(5);
			}
			PORTB &= ~LED_VS;
			USART_text_out("DREQ wait end\n\r");
			return;
		}
		_delay_ms(1);
		PORTB &= ~LED_VS;
		_delay_ms(1);
#endif
	}
	USART_text_out("DREQ wait end\n\r");
}

//#ifdef VS1011E_SPI_SOFT
//BYTE spi_byte_soft(BYTE data)
//{
	//BYTE ret = 0;
	//for (int b = 7; b >= 0; b--)
	//{
		//if (data & (1 << b))
			//PORTD |= SI;
		//else
			//PORTD &= ~SI;
//
		//PORTD |= SCLK;
//
		//ret |= ((PIND & SO) << b);
//
		//PORTD &= ~SCLK;
	//}
	//return ret;
//}
//#endif

void send_array(BYTE buf[], unsigned short len)
{
	for(int n = 0; n < len; n++) spi_send(buf[n]);

//#ifdef VS1011E_SPI_SOFT
		//spi_byte_soft(buf[n]);
//#else
//#endif
}

void vs1011e_cmd(BYTE reg, DWORD cmd)
{
#ifdef VS1011E_DATA_VALIDATE
	while(1)
	{
#endif
		BYTE buf[4];
		buf[0] = 2;
		buf[1] = reg;
		buf[2] = cmd >> 8;
		buf[3] = cmd;

		SELECT_XCS;
		USART_text_out("vs1011e write\n\r");
		send_array(buf, 4);
		wait_for_ready();
		UNSELECT_XCS;

#ifdef VS1011E_DATA_VALIDATE
		DWORD retCmd = vs1011e_read(reg);
		USART_text_out("vs1011e read\n\r");
		wait_for_ready();
		if (((short)retCmd) != ((short)cmd))
		{
			USART_text_out("Error!   reg:");
			USART_num_out(reg,2);
			USART_text_out(" cmd:");
			USART_num_out(buf[2],3);
			USART_text_out(" ");
			USART_num_out(buf[3],3);
			buf[2] = retCmd >> 8;
			buf[3] = retCmd;
			USART_text_out(" ");			
			USART_num_out(buf[2],3);
			USART_text_out(" ");
			USART_num_out(buf[3],3);
			USART_text_out("\n\r");
		}
		else
		{
			USART_text_out("Success! reg:");
			USART_num_out(reg,2);
			USART_text_out(" cmd:");
			USART_num_out(buf[2],3);
			USART_text_out(" ");
			USART_num_out(buf[3],3);
			USART_text_out("\n\r");
			return;
		}
		
	}
#endif
}

DWORD vs1011e_read(BYTE reg)
{
	BYTE cmd[2];
	cmd[0] = 0x3;
	cmd[1] = reg;

	SELECT_XCS;

	_delay_ms(1);

	send_array(cmd, 2);

#ifdef VS1011E_SPI_SOFT
	DWORD ret = spi_byte_soft(0) << 8;
	ret |= spi_byte_soft(0);
#else
	DWORD ret = spi_recv() << 8;
	ret |= spi_recv();
#endif

	_delay_ms(1);

	UNSELECT_XCS;

	return ret;
}

void set_volume()
{
	if (currentVol > 2) currentVol = 0;
	wait_for_ready();
}

void set_clock()
{	
	wait_for_ready();
}

void test_sin_start()
{
	BYTE buf[8];
	buf[0] = 0x53;
	buf[1] = 0xEF;
	buf[2] = 0x6E;
	buf[3] = 0x4A;

	for(int i=0; i<4; i++) buf[i + 4] = 0;

	SELECT_XDCS;
	send_array(buf, 8);
	UNSELECT_XDCS;
}

void test_sin_end()
{
	BYTE buf[8];
	buf[0] = 0x45;
	buf[1] = 0x78;
	buf[2] = 0x69;
	buf[3] = 0x74;

	for(int i=0; i<4; i++) buf[i + 4] = 0;

	SELECT_XDCS;
	send_array(buf, 8);
	UNSELECT_XDCS;
}
