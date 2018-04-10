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

#define DISP_CLR    0x01
#define HOME_RET    0x02
//Change the definitions below to change initialization
#define DISP_SET    0x0F //0x0F Disp ON, Cursor ON, Blink ON
#define FXN_SET     0x28 //0x28 4-Bit, 2 line, 5x8 font
#define ENTRY_SET   0x07 //0x06 -> cursor, addr++, disp++

void delay_ms(int, int);
void delay_us(int, int);
void set_DCO(int);

void LCD_INIT(int);
void LCD_CTRL(unsigned char, int);
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
    clear_LCD(CLK);
    unsigned char letter = 0x41; //ASCII A
    unsigned char addr = 0x00;  //Top-left pixel on LCD
    /*while(1) {
        write_char_LCD(letter, addr, CLK);
        delay_ms(1000, CLK);
        clear_LCD(CLK);
        home_LCD(CLK);
        delay_ms(1000, CLK);
    }*/
    int i;
    while(1) {
        for(i = 0x00; i <= 0x0F; i++) {
            write_char_LCD(letter, i, CLK);
            delay_ms(1000, CLK);
            letter++;
        }
        for(i = 0x40; i <= 0x4F; i++) {
            write_char_LCD(letter, i, CLK);
            delay_ms(1000, CLK);
            letter++;
        }
        clear_LCD(CLK);
        delay_ms(1000, CLK);
        letter  = 0x41;
        addr    = 0x00;
    }
}

//Takes in DDRAM address pixel and ASCII character sym
//0x00...0x0F DDRAM Addresses
//0x40...0x4F
void write_char_LCD(unsigned char sym, unsigned char pixel, int CLK)
{
    pixel |= DB7;
    unsigned char CTRL = EN;
    LCD_CMD(pixel, CTRL, CLK);
    delay_us(37, CLK);
    CTRL = EN | RS;
    LCD_CMD(sym, CTRL, CLK);
    delay_us(37, CLK);
}

//1.52 ms delay required after operation
void home_LCD(int CLK)
{
    LCD_CMD(HOME_RET, EN, CLK);
    delay_ms(2, CLK);
}

//1.52 ms delay required after operation
void clear_LCD(int CLK)
{
    LCD_CMD(DISP_CLR, EN, CLK);
    delay_ms(2, CLK);
}

//Sets up I/O register direction
//Runs through LCD setup procedure
void LCD_INIT(int CLK)
{
    P3 -> DIR |= RS | RW | EN;
    P4 -> DIR = 0xFF; //Using all 8 bits in reg so we can be lazy
    delay_ms(20, CLK);
    LCD_CMD(FXN_SET, EN, CLK);
    delay_us(37, CLK);
    LCD_CMD(DISP_SET, EN, CLK);
    delay_us(37, CLK);
    LCD_CMD(DISP_CLR, EN, CLK);
    delay_ms(2, CLK);
    LCD_CMD(ENTRY_SET, EN, CLK);
    delay_us(37, CLK);
}

//Sets control bits to states given in CTRL
//CTRL: Top three bits EN, RW, RS respectively
void LCD_CTRL(unsigned char CTRL, int CLK)
{
    P3 -> OUT &= ~(RS | RW | EN); //Clears RS, RW, and EN
    P3 -> OUT |= CTRL; //Sets RS, RW, and EN to desired state
    delay_us(30, CLK); //Nominal 460 ns W / 480 ns R
}

//Writes to the 8 data bits of the LCD
//Can access both data and instruction registers
//30 us delays to ensure known timings
void LCD_CMD(unsigned char CMD, unsigned char CTRL, int CLK)
{
    P4 -> OUT = CMD;
    LCD_CTRL(CTRL, CLK);
    P3 -> OUT &= ~EN;   //Nibble 1
    delay_us(30, CLK);  //Nominal 10 ns
    if ((FXN_SET & DB4) == 0x00) {
        CMD = CMD << 4;
        P4 -> OUT = CMD;
        LCD_CTRL(CTRL, CLK);
        P3 -> OUT &= ~EN;   //Nibble 2
        delay_us(30, CLK);  //Nominal 10 ns
    }
    P4 -> OUT = 0x00;
    delay_us(30, CLK); //Nominal 730 ns
}
