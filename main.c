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
#include "lcd.h"
#include "usart.h"
#define BAUD 9600                    //definiert die Bautrate für den USART.
#define USART0_RX_BUFFER_SIZE 32      //definiert die größe des Empfangsbuffers. Die Buffergröße kann 2, 4, 8, 16, 32, 64, 128 oder 256 Byte groß sein.
#define USART0_TX_BUFFER_SIZE 32      //definiert die größe des Sendebuffers. Die Buffergröße kann 2, 4, 8, 16, 32, 64, 128 oder 256 Byte groß sein.
#define ADC_CHANNELS 1

enum {
    DIR_OPEN = 0,
    DIR_CLOSE
};

volatile uint8_t adc_channel = 0;

volatile uint32_t transmission_count = 0;
const uint32_t adc_count_limit = 10000;
volatile uint32_t adc_count = 0;

uint32_t adc_sums[ADC_CHANNELS];
uint64_t adc_quads[ADC_CHANNELS];

void uart_puts (const char *s)
{
    while (*s)
    {
        usart0_putc(*s);
        s++;
    }
}

void ulltoa(uint64_t val, char buf[]) {
	uint8_t i = 0;
	while (0 != val) {
		uint8_t digit = val % 10;
		switch (digit) {
			case 0 : buf[i] = '0'; break;
			case 1 : buf[i] = '1'; break;
			case 2 : buf[i] = '2'; break;
			case 3 : buf[i] = '3'; break;
			case 4 : buf[i] = '4'; break;
			case 5 : buf[i] = '5'; break;
			case 6 : buf[i] = '6'; break;
			case 7 : buf[i] = '7'; break;
			case 8 : buf[i] = '8'; break;
			case 9 : buf[i] = '9'; break;
			default: buf[i] = '*';
		}
		i++;
		val /= 10;
	}
	buf[i] = '\0';
	buf = strrev(buf);
}

void print_uint16(uint16_t val) {
	char buf[12];
	utoa(val, buf, 10);
	uart_puts(buf);
	uart_puts(" ");
}

//*
void print_uint32(uint32_t val) {
	char buf[12];
	ultoa(val, buf, 10);
	uart_puts(buf);
	uart_puts(" ");
}

void print_uint64(uint64_t val) {
	char buf[22];
	ulltoa(val, buf);
	uart_puts(buf);
	uart_puts(" ");
}

void print_int32(int32_t val) {
	char buf[12];
	ltoa(val, buf, 10);
	uart_puts(buf);
	uart_puts(" ");
}

/* ADC initialisieren */
void adc_init(void) {
				 
	uint16_t result;
					  
	// Disable digital input.
	//DIDR0 = 0b11111111;

	//  ADMUX = (0<<REFS1) | (1<<REFS0);      // AVcc als Referenz benutzen
	ADMUX =  (0<<REFS1) | (1<<REFS0);      // interne Referenzspannung nutzen
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
	ADCSRA |= (1<<ADEN);                  // ADC aktivieren
									 
	/* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
	also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
													 
	ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
	while (ADCSRA & (1<<ADSC) ) {}        // auf Abschluss der Konvertierung warten
	/* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
	Wandlung nicht übernommen. */
	 result = ADCW;
}


void calculate_temperatures() {
	transmission_count++;
	print_uint32(transmission_count);
	print_uint32(adc_count_limit);
	for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
		print_uint32(adc_sums[i]);
		print_uint64(adc_quads[i]);
	}

	uart_puts("\r\n");
	const double a_s[8] = {0.606903, 0.650287, 0.623272, 0.639654, 0.629204, 0.620277, 0.616827, 0.623187};
	const double b_s[8] = {-274.037, -274.037, -280.540, -288.488, -285.116, -280.541, -279.017, -278.405};

	for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
		double precision = 1000.0;
		double val;
		val = adc_sums[i];
		val *= precision * a_s[i] / adc_count_limit;
		val -= precision * b_s[i];
	}

	//uart_puts("\r\n\r\n");
	for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
		adc_sums[i] = 0;
		adc_quads[i] = 0;
	}
}

ISR(ADC_vect) {
	usart0_putc('P');
    uint32_t value = ADC;
		uint8_t old_channel = adc_channel;

		adc_channel = (adc_channel + 1) % ADC_CHANNELS;
    /* ADC messen */
    ADMUX = (1 << REFS0) | adc_channel;

    ADCSRA = (1 << ADEN) | (1 << ADIF) | (1 << ADIE) | (1 << ADPS2);
    /* start */
    ADCSRA |= (1 << ADSC);
	
		adc_sums[old_channel] += value;
		adc_quads[old_channel] += value * value;
		if (old_channel == ADC_CHANNELS-1) {
			adc_count++;
			if (adc_count == adc_count_limit) {
				adc_count = 0;
				calculate_temperatures();
			}
		}

}


int main(void)
{

    usart0_init();
		
	/* enable LED so that the user knows the controller is active */
    DDRC = (1 << PC7);
    PORTC = (1 << PC7);
    
	sei();
	
	adc_init();

	_delay_ms(500);
    PORTC = 0;
		uart_puts("\r\n#AFDH\r\n");
    _delay_ms(500);
    PORTC = (1 << PC7);
		uart_puts("#AFDH\r\n");
    _delay_ms(500);
		
		/* ADC messen */
		ADMUX = (1 << REFS0) | adc_channel;

		ADCSRA = (1 << ADEN) | (1 << ADIF) | (1 << ADIE) | (1 << ADPS2);
			//DIDR0 = 0xFF;
		/* start */
		ADCSRA |= (1 << ADSC);

		for (;;) {
		}

}


