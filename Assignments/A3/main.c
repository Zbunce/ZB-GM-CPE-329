/**
 * main.c
 *
 * Date: April 9th, 2018
 */

//Runs LCD in 8-bit mode

//To run in 4-bit you have to write data twice?
#include "msp.h"
#include "delays.h"
#include "set_DCO.h"

void delay_ms(int, int);
void delay_us(int, int);
void set_DCO(int);

void LCD_INIT(int);
void LCD_CMD(unsigned char, unsigned char, int);
void write_char_LCD(unsigned char, unsigned char, int);
void clear_LCD(int);
void home_LCD(int);

void main()
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int CLK = 480;

	set_DCO(CLK);
    LCD_INIT(CLK);
    unsigned char letter  = 0x41;
    unsigned char addr    = 0x00;

    //Writes an "A" to the top left pixel, and then clears it.
    while(1) {
        write_char_LCD(letter, addr, CLK);
        delay_ms(500, CLK);
        clear_LCD(CLK);
        delay_ms(500, CLK);
    }
    /*//Writes consecutive characters through all pixels
    //Then resets
    unsigned char i;
    while(1) {
        clear_LCD(CLK);
        delay_ms(1000, CLK);
        letter  = 0x41;
        addr    = 0x00;
        for(i = 0x00; i <= 0x0F; i++) {
            write_char_LCD(letter, i, CLK);
            delay_ms(500, CLK);
            letter++;
        }
        for(i = 0x40; i <= 0x4F; i++) {
            write_char_LCD(letter, i, CLK);
            delay_ms(500, CLK);
            letter++;
        }
    }*/
}
