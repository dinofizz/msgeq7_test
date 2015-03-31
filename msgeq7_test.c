/**
* @file msgeq7_test.c
* @brief This file is used to test that the microcontroller
* is able to read the the output of the MSGEQ7 correctly.
* @author Dino Fizzotti
* @version 0.1
* @date 2015-03-31
*/
#define F_CPU 18432000UL

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#define output_low(port,pin) port &= ~(1 << pin)
#define output_high(port,pin) port |= (1 << pin)
#define set_input(portdir,pin) portdir &= ~(1 << pin)
#define set_output(portdir,pin) portdir |= (1 << pin)
#define toggle_pin(port,pin) port ^= (1 << pin);

#define RESET_HIGH output_high(PORTC, MSGEQ7_RESET) 
#define RESET_LOW output_low(PORTC, MSGEQ7_RESET) 
#define STROBE_HIGH output_high(PORTC, MSGEQ7_STROBE) 
#define STROBE_LOW output_low(PORTC, MSGEQ7_STROBE) 

#define TIMER_FREQ 500

#define MSGEQ7_OUTPUT PINA0
#define MSGEQ7_RESET PINC1
#define MSGEQ7_STROBE PINC2

SIGNAL(TIMER1_COMPA_vect)
{
    RESET_HIGH;
    STROBE_HIGH;
    _delay_us(1);
    RESET_LOW;
    _delay_us(80);

    char freq_array[7];

    for (int i = 0; i < 7; i++)
    {
        STROBE_LOW;
        _delay_us(40);

        ADCSRA |= (1 << ADSC);

        while(ADCSRA & (1<<ADSC));

        freq_array[i] = ADCH;
        
        STROBE_HIGH;
        _delay_us(20);
    }

    char abc = freq_array[1];

    // Flashes the LEDs on the STK500 (connected to PORTB)
    // if the 2nd lowest frequency is above a threshold
    // (thumbsucked).
    if (abc > 63)
    {
        PORTB = 0x00;
    }
    else
    {
        PORTB = 0xFF;
    }
}

void setup_timer()
{
    TCCR1A = 0x00;
    TCCR1B |= (1 << CS01) | (1 << WGM12); 
    TIMSK1 = (1<<OCIE1A);   
    OCR1A = (F_CPU / 8) / TIMER_FREQ;
}

void setup_adc()
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADMUX |= (1 << ADLAR);
    ADCSRA |= (1 << ADEN);
}

void setup_pins()
{
    set_input(DDRA, MSGEQ7_OUTPUT);
    set_output(DDRC, MSGEQ7_RESET);
    set_output(DDRC, MSGEQ7_STROBE);
    DDRB = 0xFF;
}

int main(void)
{
    setup_pins();
    setup_timer();
    setup_adc();
    sei();
    while(1);
    return 0; // keep compiler happy
}
