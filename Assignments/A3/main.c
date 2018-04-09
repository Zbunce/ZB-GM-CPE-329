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
#define FXN_SET     0x38 //8-Bit, 2 line, 5x8 font spec.

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	LCD_INIT();

    LCD_CMD(FXN_SET);
    delay_us(37, SYS_FREQ);
}

//Sets up I/O register direction
void LCD_INIT(void)
{
    P3 -> DIR |= RS | RW | EN;
    P4 -> DIR = 0xFF; //Using all 8 bits in reg so we can be lazy
}

//Writes character to LCD
void LCD_CMD(unsigned char CMD)
{
    P3 -> OUT &= ~(RS+RW+EN);
    P4 -> OUT = 0x00;
    P3 -> OUT |= EN;
    delay_us(100, SYS_FREQ);
    P4 -> OUT = CMD;
    delay_us(10, SYS_FREQ);
    P3 -> OUT &= ~EN;
    P4 -> OUT = 0x00;
    delay_us(1, SYS_FREQ);
}

