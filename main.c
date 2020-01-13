/*
 * Luxcius_laser_pointer.c
 *
 * Created: 2019/09/30 1:07:51
 * Author : yoda
 */ 

#include <avr/io.h>
#include "spi_atmega88.h"
#include "usart_atmega88.h"
#include "I2C_atmega88.h"

#include "vs1011e.h"

/* define */
// delay time parameter
#define LASER_DELAY 3000 //ms
#define MODE_CHANGE_DELAY 1000 //ms
#define LASER_INTERVAL 12//sec

// sound effect parameter
#define INITIAL_ADDRESS 0x0000
#define SE_BYTES 15232

// output
#define LASER_L_ON	(PORTD|=0x10)
#define LASER_L_OFF	(PORTD&=0xEF)
#define LASER_R_ON	(PORTD|=0x80)
#define LASER_R_OFF	(PORTD&=0x7F)
#define LED_TEST_ON		(PORTB|=0x01)
#define LED_TEST_OFF	(PORTB&=0xFE)

// input
#define	SW_LASER	(PINC&0x01)
#define	SW_MODE		(PINC&0x02)

// base define
#define LEFT  0
#define RIGHT 1

#define ON  1
#define OFF 0


/* function list */
void initialize(void);

void laser_sound_effect( void );

void eye_LED_delay_ON(short ms);
void eye_LED_OFF(void);

unsigned short EEPROM_write( unsigned short head_add );
char EEPROM_read( unsigned short head_add, unsigned short read_bytes );

char sw_mode_det(void);
char sw_laser_det(void);

void interval_sec(unsigned char sec);


void initialize(void)
{
	DDRB	= 0b11101101; // PORTB 7:xCS 6:xDCS 5:SCK 4:MISO 3:MOSI 2:SS 1:DREQ 0:MP3_LED
	PORTB	= 0b11000100; // initial output/pull up setting
	DDRC	= 0b00110000; // PORTC 5:SCL 4:SDA 1:SW_MODE 0:SW_LASER
	PORTC	= 0b00110011; // initial output/pull up setting
	DDRD	= 0b11010111; // PORTD 7:LASERR 6:EYEPWM 4:LASERL 2:MP3RST 1:TXD 0:RXD
	PORTD	= 0b00000000; // initial output/pull up setting
	
	// PWM setting
	TCCR0A = 0b10000011;  //7-6:1,0 compareLOW  1-0:10 ‚‘¬PWM“®ì
	TCCR0B = 0b00000001;  //3:0 ‚‘¬PWM“®ì  2-0:001 •ªŽü‚È‚µ
	OCR0A = 0; // duty ratio: OCR0A/255

	/* USART init */
	USART_init(MYUBRR);
	USART_text_out("-----initialize end-----\n\r");
	
	/* I2C init */
	TWBR = 12; // 2: scl 400000 kHz 8MHz/(16+2*TWBR*TWPS)
	
	/* SPI init */
	spi_master_init(4);
	
	/* mp3 decoder */
	vs1011e_init();
}


int main(void)
{
	initialize();

	// EEPROM write read 
	//num = EEPROM_write( add );
	//EEPROM_read( add, num );

	while(1)
	{
		/* BETA MODE */
		while( sw_mode_det()==OFF )
		{
			if(SW_LASER == 0)
			{
				eye_LED_delay_ON(LASER_DELAY);
				LASER_L_ON;
				LASER_R_ON;
				laser_sound_effect();
				while(SW_LASER==0); // wait sw_laser off
				LASER_L_OFF;
				LASER_R_OFF;
				eye_LED_OFF();
				interval_sec(LASER_INTERVAL);
			}			
		}
		// mode change
		_delay_ms(500);
		eye_LED_delay_ON(MODE_CHANGE_DELAY);
		eye_LED_OFF();
		
		
		/* SIMPLE LEFT MODE */
		while( sw_mode_det()==OFF )
		{
			if(SW_LASER==0)LASER_L_ON;
			else LASER_L_OFF;
		}
		// mode change
		_delay_ms(500);
		eye_LED_delay_ON(MODE_CHANGE_DELAY);
		eye_LED_OFF();
		
		
		/* SIMPLE RIGHT MODE */
		while( sw_mode_det()==OFF )
		{
			if(SW_LASER==0)LASER_R_ON;
			else LASER_R_OFF;
		}
		// mode change
		_delay_ms(500);
		eye_LED_delay_ON(MODE_CHANGE_DELAY);
		eye_LED_OFF();
	}
}



// laser sound effect
void laser_sound_effect( void )
{
	char res;
	unsigned char  buf[PAGE_BYTE];
	unsigned short add = INITIAL_ADDRESS;
	unsigned short i=0;

	//LED_TEST_ON;
	wait_for_ready(); 	// vs1011e ready
	
	/* SE data send -> vs1011e */
	SELECT_XDCS;
	while( add <= SE_BYTES )
	{
		//USART_text_out("address:");
		//USART_num_out(add, 5);
		//USART_text_out("\n\r");
		
		/* EEPROM read */
		res = i2c_page_read( add, buf );
		if( res != OK )
		{
			USART_text_out("read error:");
			USART_num_out(res, 1);
			USART_text_out("\n\r");
			break;
		}
				
		/* vs1011e send */
		for(i=0; i<PAGE_BYTE; i++)
		{
			if( (add+i) > SE_BYTES ) break;
			spi_send(buf[i]);
			//USART_hex_out(buf[i]);
			//USART_text_out("\n\r");
		}
		add += PAGE_BYTE;
	}
	UNSELECT_XDCS;
}

// eye led delay on function
// ms:min 100
#define MAX_STEP 100
void eye_LED_delay_ON(short ms)
{
	short i,step;
	step = ms/MAX_STEP;
	for(i=0; i<MAX_STEP; i=i+2)
	{
		OCR0A = i;
		_delay_ms(step);
	}
}

void eye_LED_OFF(void)
{
	OCR0A = 0;
}

// EEP-ROM write mode( teraterm setting: delay 1ms)
unsigned short EEPROM_write( unsigned short head_add )
{
	char res;
	unsigned char recv, recv_hex, dat;
	unsigned short ircv=0, iad=0;
	
	USART_text_out("EEPROM data wait...(END:LASER_SW)\n\r");
	while( SW_LASER )
	{
		// data wait
		recv = USART_recv();
		
		// receive data recognition
		ircv++;
		if( recv == '0' ) recv_hex = 0x0;
		else if ( recv == '1' ) recv_hex = 0x1;
		else if ( recv == '2' ) recv_hex = 0x2;
		else if ( recv == '3' ) recv_hex = 0x3;
		else if ( recv == '4' ) recv_hex = 0x4;
		else if ( recv == '5' ) recv_hex = 0x5;
		else if ( recv == '6' ) recv_hex = 0x6;
		else if ( recv == '7' ) recv_hex = 0x7;
		else if ( recv == '8' ) recv_hex = 0x8;
		else if ( recv == '9' ) recv_hex = 0x9;
		else if ( recv == 'A' ) recv_hex = 0xA;
		else if ( recv == 'B' ) recv_hex = 0xB;
		else if ( recv == 'C' ) recv_hex = 0xC;
		else if ( recv == 'D' ) recv_hex = 0xD;
		else if ( recv == 'E' ) recv_hex = 0xE;
		else if ( recv == 'F' ) recv_hex = 0xF;
		else if ( recv == ' ' ) ircv = 0;
		
		if( ircv==1 ) dat |= (recv_hex << 4);
		else if( ircv==2 )
		{
			dat |= recv_hex;
			res = i2c_data_write(head_add+iad, dat);
			if( res != 0 ) // Error
			{
				USART_text_out("write error:");
				USART_num_out(res, 1);
				USART_text_out("\n\r");
				return res;
			}
			USART_num_out(iad+1, 5);
			USART_text_out("\r");
			iad++;
			dat = 0;
		}
	}
	USART_text_out("write bytes:");
	USART_num_out(iad+1, 5);
	USART_text_out("\n\r");
	return iad;
}

// EEP-ROM read
char EEPROM_read( unsigned short head_add, unsigned short read_bytes )
{
	char res;
	unsigned char rdata[PAGE_BYTE];
	unsigned short i=0, iad=0;
	
	USART_text_out("read data:\n\r");
	while( i<=read_bytes )
	{
		//res = i2c_data_read(head_add+iad, &rdata);
		res = i2c_page_read( head_add+i, rdata );
		if ( res != OK )
		{
			USART_text_out("read error:");
			USART_num_out(res, 1);
			USART_text_out("\n\r");
			return res;
		}

		for(iad=0; iad<PAGE_BYTE; iad++)
		{
			USART_hex_out( rdata[iad] );
			if( ((i+iad)%16)==15 ) USART_text_out("\n\r");
		}
		i += iad;
	}
	return OK;
}


/* switch laser detection */
char sw_laser_det(void)
{
	if(SW_LASER==0)
	{
		_delay_ms(10);
		if(SW_LASER==0)
		{
			_delay_ms(10);
			if(SW_LASER==0)
			{
				while( SW_LASER==0 );
				_delay_ms(100);
				return ON;
			}
		}
	}
	return OFF;
}

/* switch mode detection */
char sw_mode_det(void)
{
	if(SW_MODE == 0)
	{
		_delay_ms(10);
		if(SW_MODE == 0)
		{
			_delay_ms(10);
			if(SW_MODE == 0)
			{
				while(SW_MODE == 0);
				_delay_ms(100);
				return ON;
			}
		}
	}
	return OFF;
}

/* laser interval */
void interval_sec(unsigned char sec)
{
	unsigned char i;
	for(i=0; i<sec; i++) _delay_ms(1000);
}