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

#define RS          BIT5
#define RW          BIT6
#define EN          BIT7

#define DB0         BIT0
#define DB1         BIT1
#define DB2         BIT2
#define DB3         BIT3
#define DB4         BIT4
#define DB5         BIT5
#define DB6         BIT6
#define DB7         BIT7

#define CLR_DISP    0x01
#define FXN_SET     0x28
#define HOME_RET    0x02
//0x28 4-Bit, 2 line, 5x8 font, 0x388-Bit, 2 line, 5x8 font

void main()
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	delay_ms(20, CLK);
    LCD_INIT();

    LCD_CMD(FXN_SET);
    delay_us(37, CLK); //Nominal 37us, but delay_us only reliable above 55us
    LCD_CMD()
}

//Takes in DDRAM address pixel and ASCII character sym
void write_char_LCD(unsigned char pixel, unsigned char sym, int CLK)
{
    unsigned char addr = 0x80 | sym;
    LCD_CMD(addr, CLK);
    delay_us(37, CLK);
    
    
    LCD_CMD()
}

//1.52 ms delay required after operation
void home_LCD(int CLK)
{
    LCD_CMD(HOME_RET);
    delay_ms(2, CLK);
}

//1.52 ms delay required after operation
void clear_LCD(int CLK)
{
    LCD_CMD(CLR_DISP);
    delay_ms(2, CLK);
}

//Sets up I/O register direction
void LCD_INIT()
{
    P3 -> DIR |= RS | RW | EN;
    P4 -> DIR = 0xFF; //Using all 8 bits in reg so we can be lazy
}

//CTRL: Top three bits EN, RW, RS respectively
void LCD_CTRL(unsigned int CTRL, int CLK)
{
    P3 -> OUT &= ~(RS+RW+EN);
    P3 -> OUT |= CTRL;
    delay_us(100, CLK);
}

//Writes character to LCD
void LCD_CMD(unsigned char CMD, int CLK)
{
    P4 -> OUT = 0x00;
    P4 -> OUT = CMD;
    delay_us(10, CLK);
    P3 -> OUT &= ~EN;
    P4 -> OUT = 0x00;
    delay_us(1, CLK);
}

