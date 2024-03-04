#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "usart.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>

// #define STEPS_PER_REVOLUTION 200
#define DELAY_BOTTOM 1100 // 700 //1200
#define DELAY_TOP 1100    // 400
#define XSTEP 1000
#define YSTEP 1000
// DDRC 0-3 -> buttons

// const float Y_DISTANCE_PER_STEP 49.164;
// const float X_DISTANCE_PER_STEP 9.091;

void x_pos(int);
void x_neg(int);
void y_pos(int);
void y_neg(int);

int automatic = 0;

int main()
{
    uart_init();
    io_redirect();
    int x = 1;
    int y = 1;

    // D2 (X), D3 (Y), D5(X_DIR), D6 (Y_DIR) - output
    DDRD = 0x6C;
    PORTD = 0x00;
    DDRC &= ~0b00001111; // pc1 input for one switch connected to C1 port
    PORTC |= 0b00001111; // enabling pullups

    while (1)
    {

        if (PINC == 0b00000000)
        {
            automatic = 1;
            printf("Automatic set to 1.");
            //(PINC &= ((~(1 << PINC0)) & (~(1 << PINC1)) & (~(1 << PINC2)) & (~(1 << PINC3))))
        }

        while (PINC == 0b00001110)
        {
            // button 1
            // x_pos(10);
            PORTD |= ((1 << PD2) | (1 << PD5) | (1 << PD6));
            _delay_us(DELAY_BOTTOM);
            PORTD &= ~(1 << PD2);
        }
        while (PINC == 0b00001101)
        {
            // !(PINC |= ~(1 << PINC1))
            // button 2
            x_neg(1);
        }
        while (PINC == 0b00001011)
        {
            // button 3
            y_pos(1);
        }
        while (PINC == 0b00000111)
        {
            // button
            y_neg(1);
        }

        // if all 4 are pressed
        if (automatic == 1)
        {
            x_pos(1115);
            y_neg(580);
            x_pos(5090);
            y_neg(1168);
            x_neg(607);
            y_neg(1143);
            x_neg(590);
            y_neg(840);
            x_neg(1574);
            y_pos(1923);
            x_neg(1273);
            y_neg(2928);
            x_pos(500);
            y_neg(583);
            x_pos(1944);
            y_neg(633);
            x_pos(1625);
            y_pos(1334);
            x_pos(524);
            y_neg(1940);
            x_pos(285);

            x_neg(285);
            y_pos(1940);
            x_neg(524);
            y_neg(1334);
            x_neg(1625);
            y_pos(633);
            x_neg(1944);
            y_pos(583);
            x_neg(500);
            y_pos(2928);
            x_pos(1273);
            y_neg(1923);
            x_pos(1574);
            y_pos(840);
            x_pos(590);
            y_pos(1143);
            x_pos(607);
            y_pos(1168);
            x_neg(5090);
            y_pos(580);
            x_neg(1115);

            automatic = 0;
        }
    }

    return 0;
}

void x_pos(int steps)
{
    int x;
    for (x = 1; x <= steps; x++)
    {
        // Turn on X motor, set X dir to +
        PORTD |= ((1 << PD2) | (1 << PD5) | (1 << PD6));
        _delay_us(DELAY_BOTTOM);
        PORTD &= ~(1 << PD2);
    }
}
void x_neg(int steps)
{
    int x;
    for (x = 1; x <= steps; x++)
    {
        // Set X dir to -
        PORTD &= ((~(1 << PD5)) & (~(1 << PD6)));
        // Turn on X motor
        PORTD |= (1 << PD2);
        _delay_us(DELAY_BOTTOM);
        PORTD &= ~(1 << PD2);
    }
}
void y_pos(int steps)
{
    int y;
    for (y = 1; y <= steps; y++)
    {
        // Turn on Y motor, set Y dir to +
        PORTD |= ((1 << PD3) | (1 << PD5) | (1 << PD6));
        _delay_us(DELAY_TOP);
        PORTD &= ~(1 << PD3);
    }
}
void y_neg(int steps)
{
    int y;
    for (y = 1; y <= steps; y++)
    {
        // set Y dir to -
        PORTD &= ((~(1 << PD5)) & (~(1 << PD6)));
        // Turn on Y motor
        PORTD |= (1 << PD3);
        _delay_us(DELAY_TOP);
        PORTD &= ~(1 << PD3);
    }
}
