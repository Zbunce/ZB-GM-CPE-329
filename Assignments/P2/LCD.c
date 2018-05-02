/*
 * LCD.c
 * Code file for LCD control.
 * Functions designed for external use:
 * write_char_LCD, clear_LCD, & home_LCD
 *
 * Date: April 10, 2018
 * Authors: Zach Bunce, Garret Maxon
 */

#include "msp.h"
#include "delays.h"
#include "LCD.h"
#include <string.h>

void LCD_CMD(uint8_t, uint8_t, int);
void LCD_CTRL(uint8_t, int);
void write_char_LCD(uint8_t, uint8_t, int);
void clear_LCD(int);

//Takes in DDRAM address pixel and ASCII character sym
//0x00...0x0F DDRAM Addresses
//0x40...0x4F
void write_char_LCD(uint8_t sym, uint8_t pixel, int CLK)
{
    pixel |= DB7;
    uint8_t CTRL = EN;
    LCD_CMD(pixel, CTRL, CLK);
    delay_us(37, CLK);
    CTRL = EN | RS;
    LCD_CMD(sym, CTRL, CLK);
    delay_us(37, CLK);
}

//Takes in DDRAM address pixel and ASCII string word
//Word wraps if line ends are reached
void write_string_LCD(char word[], uint8_t pixel, int CLK)
{
    uint8_t i;
    uint8_t location = pixel;
    uint8_t len = strlen(word);
    for(i = 0; i < len; i++)
    {
        if ((location > 0x0F) && (location < 0x40)) {
            location = 0x40;
        }
        else if (location > 0x4F) {
            location = 0x00;
        }
        write_char_LCD(word[i], location, CLK);
        location++;
    }
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

void line_clear_LCD(int line, int CLK)
{
    char blank[] = "                ";
    if (line == TOP) {
        write_string_LCD(blank, 0x00, CLK);
        home_LCD(CLK);
    }
    else if (line == BOTTOM) {
        write_string_LCD(blank, 0x40, CLK);
        write_char_LCD(0x10, 0x3F, CLK);
    }
}

//Sets up I/O register direction
//Runs through LCD setup procedure
//USE LCD_3.3V_LCD_NHD DATASHEET PROCEDURE
//Currently only works for 4-bit mode; scared of timings
void LCD_INIT(int CLK)
{
    P3 -> DIR |= RS | RW | EN;  //Sets reg directions
    P4 -> DIR |= DB7 | DB6 | DB5 | DB4; //Not using all 4 so don't be lazy

    P3 -> OUT &= ~(RS | RW | EN);
    P4 -> OUT &= ~(DB7 | DB6 | DB5 | DB4); //Sets output low
    delay_ms(40, CLK);  //Waits for safe power up
    P4 -> OUT |= 0x30;   //Sets wake up command
    delay_ms(5, CLK);

    LCD_CTRL(EN, CLK);  //Wake up #1
    P3 -> OUT &= ~EN;
    delay_us(160, CLK);

    LCD_CTRL(EN, CLK);  //Wake up #2
    P3 -> OUT &= ~EN;
    delay_us(160, CLK);

    LCD_CTRL(EN, CLK);  //Wake up #3
    P3 -> OUT &= ~EN;
    delay_us(160, CLK);

    P4 -> OUT &= ~(DB7 | DB6 | DB5 | DB4); //Sets output low
    P4 -> OUT |= 0x20;   //Guess we're awake
    LCD_CTRL(EN, CLK);
    P3 -> OUT &= ~EN;

    //The rest sets up LCD settings
    LCD_CMD(FXN_SET,    EN, CLK);
    LCD_CMD(SHIFT_SET,  EN, CLK);
    LCD_CMD(DISP_SET,   EN, CLK);
    LCD_CMD(ENTRY_SET,  EN, CLK);

    clear_LCD(CLK); //Bill Murray
}

//Sets control bits to states given in CTRL
//CTRL: Top three bits EN, RW, RS respectively
void LCD_CTRL(uint8_t CTRL, int CLK)
{
    P3 -> OUT &= ~(RS | RW | EN); //Clears RS, RW, and EN
    P3 -> OUT |= CTRL; //Sets RS, RW, and EN to desired state
    delay_us(30, CLK); //Nominal 460 ns W / 480 ns R
}

//Writes to the 8 data bits of the LCD
//Can access both data and instruction registers
//30 us delays to ensure known timings
void LCD_CMD(uint8_t CMD, uint8_t CTRL, int CLK)
{
    P4 -> OUT &= ~(DB7 | DB6 | DB5 | DB4); //Sets output low
    P4 -> OUT |= CMD & 0xF0;
    LCD_CTRL(CTRL, CLK);
    P3 -> OUT &= ~EN;   //Nibble 1
    delay_us(30, CLK);  //Nominal 10 ns
    if ((FXN_SET & DB4) == 0x00) {
        CMD = CMD << 4;
        P4 -> OUT &= ~(DB7 | DB6 | DB5 | DB4); //Sets output low
        P4 -> OUT |= CMD & 0xF0;
        LCD_CTRL(CTRL, CLK);
        P3 -> OUT &= ~EN;   //Nibble 2
        delay_us(30, CLK);  //Nominal 10 ns
    }
    P4 -> OUT &= ~(DB7 | DB6 | DB5 | DB4);
    delay_us(80, CLK); //Nominal 730 ns
}
