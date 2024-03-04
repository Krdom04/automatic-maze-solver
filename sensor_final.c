#define F_CPU 16000000UL
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "i2cmaster.h"
#include "lcd.h"
#include "usart.h"
#include "lm75.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <math.h>
/*
#define BAUDRATE 57600                                      // The baudratethat we want to use
#define BAUD_PRESCALER (((F_CPU / (BAUDRATE * 16UL))) - 1) // This calculates the BAUD_Prescalerfor us
*/

volatile long duration;
volatile int distance;
volatile int n;
volatile int k;
uint16_t max_time;
unsigned int addr = 0;

int calculate_distance();
void inter_setup();

int main()
{
    i2c_init();
    uart_init();
    io_redirect();
    // Set trig (D5) as output, echo (D6) as input
    DDRD = 0x20;
    // Clear pins
    PORTD = 0x00;
    inter_setup();

    while(1){
        // Set trig low for 2us
        PORTD &= ~(1 << PD5);
        _delay_us(2);
        // Set trig high for 10us
        PORTD |= (1 << PD5);
        _delay_us(10);
        // Set trig low
        PORTD &= ~(1 << PD5);
        
        
        while((TIFR1 & (1 << OCF1A) == 0) || (TCCR1B & (1 << CS12)) == 1){
            // Wait for overflow or timer to be stopped
        }
        
        // Echo goes high, if t_HL == 38ms (overflow occurs) - no object
        
        if( (TIFR1 & (1 << OCF1A)) != 0){
            printf("No object nearby \n");
            // Reset overflow flag
            TIFR1 = (1 << OCF1A);
        }
        // t_HL < 38ms - object present, calculate distance
        else if(max_time != 0){
            printf("Distance: %d cm \n", calculate_distance());
        }
        _delay_ms(150);
    }
    return 0;
}

void inter_setup(){
    // Any logic change on pins PCINT[23:16] to cause an interrupt
    PCICR |= (1 << PCIE2);
    // Logic change on specifically PCINT22 to cause an interrupt
    PCMSK2 |= (1 << PCINT22);
    // enable interrupts
    sei();
}

ISR(PCINT2_vect){
    // if logic change, then this happens
    if ( (PIND & (1 << PIND6)) ){
        /* LOW to HIGH */
        // start timer
        // Set timer1 to CTC
        max_time = 0;
        TCCR1A |= (1 << WGM11);
        // Set the value of OCR
        OCR1A = 0x0947;
        // Start timer, set prescaler to 256
        TCCR1B |= (1 << CS12) ;
    }
    else if( !(PIND & (1 << PIND6))){
        /* HIGH to LOW */
        // Stop timer
        TCCR1B &= ~(1 << CS12);
        // Save time when it stopped
        eeprom_write_word((uint16_t *)addr, TCNT1);
        max_time = eeprom_read_word((uint16_t *)addr);
        // Set timer value back to 0
        TCNT1 = 0x0000;
    }
}

int calculate_distance(){
    // gives time in ms
    float time = ((max_time + 1) * 0.000016);
    return ((time * 34300) /2);
}