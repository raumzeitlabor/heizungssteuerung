/*************************************************************************
Title:    testing output to a HD44780 based LCD display.
Author:   Peter Fleury  <pfleury@gmx.ch>  http://jump.to/fleury
File:     $Id: test_lcd.c,v 1.6 2004/12/10 13:53:59 peter Exp $
Software: AVR-GCC 3.3
Hardware: HD44780 compatible LCD text display
          ATS90S8515/ATmega if memory-mapped LCD interface is used
          any AVR with 7 free I/O pins if 4-bit IO port mode is used
**************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include "usart.h"
#include "onewire.h"
#include "ds18x20.h"

#define BAUD 9600                    //definiert die Bautrate für den USART.
#define USART0_RX_BUFFER_SIZE 32      //definiert die größe des Empfangsbuffers. Die Buffergröße kann 2, 4, 8, 16, 32, 64, 128 oder 256 Byte groß sein.
#define USART0_TX_BUFFER_SIZE 32      //definiert die größe des Sendebuffers. Die Buffergröße kann 2, 4, 8, 16, 32, 64, 128 oder 256 Byte groß sein.

void usart0_puts (const char *s)
{
    while (*s)
    {
        usart0_putc(*s);
        s++;
    }
}

int main(void)
{
	uint8_t sensor_id[OW_ROMCODE_SIZE];
	uint8_t diff;
	int16_t decicelsius;
	uint8_t puffer[20];
	double output;
	
    usart0_init();
		
	/* enable LED so that the user knows the controller is active */
    DDRC = (1 << PC7);
    PORTC = (1 << PC7);

	sei();

	usart0_puts("\r\nBooting up...\r\n");

#ifndef OW_ONE_BUS
	ow_set_bus(&PINB,&PORTB,&DDRB,PB0);
#endif
    ow_reset();
    DS18X20_find_sensor( &diff, &sensor_id[0] );
    
    if( diff == OW_PRESENCE_ERR ) {
		usart0_puts( "Boot Error: No Sensor found\r\n" );
		while ( 1 ) { asm volatile ("nop"); };
	}
	
	if( diff == OW_DATA_ERR ) {
		usart0_puts( "Boot Error: Bus Error\r\n" );
		while ( 1 ) { asm volatile ("nop"); };
	}

	while ( 1 ) {
		if ( DS18X20_start_meas( DS18X20_POWER_EXTERN, 
				&sensor_id[0] ) == DS18X20_OK ) {
			_delay_ms( DS18B20_TCONV_12BIT );
			if ( DS18X20_read_decicelsius( &sensor_id[0], &decicelsius) 
				     == DS18X20_OK ) {
				output = decicelsius / 10.0;
				sprintf( puffer, "%.2f", output );
				usart0_puts( puffer );
				usart0_puts("\n");
			}
		}
		_delay_ms(60000);
	}
}


