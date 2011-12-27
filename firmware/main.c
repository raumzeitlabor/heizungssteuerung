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

/* ADC initialisieren */
void ADC_Init(void) {
 
  uint16_t result;
 
//  ADMUX = (0<<REFS1) | (1<<REFS0);      // AVcc als Referenz benutzen
  ADMUX = (1<<REFS0) | (1<<REFS0);      // interne Referenzspannung nutzen
  // Bit ADFR ("free running") in ADCSRA steht beim Einschalten
  // schon auf 0, also single conversion
  ADCSRA = (1<<ADPS2) |(1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
  ADCSRA |= (1<<ADEN);                  // ADC aktivieren
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
 
  ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
  while (ADCSRA & (1<<ADSC) ) {}        // auf Abschluss der Konvertierung warten
  /* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
     Wandlung nicht übernommen. */
  result = ADCW;
}
 
/* ADC Einzelmessung */
uint16_t ADC_Read( uint8_t channel )
{
  // Kanal waehlen, ohne andere Bits zu beeinflußen
  ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
  ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
  while (ADCSRA & (1<<ADSC) ) {}  // auf Abschluss der Konvertierung warten
  return ADCW;                    // ADC auslesen und zurückgeben
}
 
/* ADC Mehrfachmessung mit Mittelwertbbildung */
uint16_t ADC_Read_Avg( uint8_t channel, uint8_t average )
{
  uint32_t result = 0;
 
  for (uint8_t i = 0; i < average; ++i )
    result += ADC_Read( channel );
 
  return (uint16_t)( result / average );
}

int main(void)
{

    usart0_init();
		
	/* enable LED so that the user knows the controller is active */
    DDRC = (1 << PC7);
    PORTC = (1 << PC7);
    
	sei();
	
	usart0_puts("\r\nBooting up...\r\n");
	
uint16_t adcval;
double sum;
  ADC_Init();
 uint8_t puffer[20];
const double a_s[8] = {0.386211, 0.413819, 0.396628, 0.407052, 0.400403, 0.394722, 0.392526, 0.396574};
	const double b_s[8] = {-166.933, -179.402, -171.071, -176.103, -173.983, -171.072, -170.102, -169.712};
   double output = 0;
   uint8_t channel = 0;
  while( 1 ) {
    adcval = ADC_Read_Avg(channel, 5);  // Kanal 2, Mittelwert aus 4 Messungen
    output = adcval * a_s[channel];
    output += b_s[channel];
    sum += output;

    

    channel++;
    if (channel > 3) {
		channel = 0;
		
		output = sum / 4;
		sprintf( puffer, "%.2f", output );
		usart0_puts( puffer );
		usart0_puts("\n");
		
		sum = 0;
		
		_delay_ms(60000);
	}
  }

}


