/*
 * LCD.h
 * Header file for LCD control.
 *
 * Date: April 10, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef LCD_H_
#define LCD_H_

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

#define BOTTOM      1       //Defines top and bottom row indicators
#define TOP         0

#define DISP_CLR    0x01
#define HOME_RET    0x02
//Change the definitions below to change initialization
#define DISP_SET    0x0F    //0x0F Disp ON, Cursor ON, Blink ON
#define FXN_SET     0x28    //0x28 4-Bit, 2 line, 5x8 font
#define SHIFT_SET   0x10    //0x10 shifts cursor or disp
#define ENTRY_SET   0x06    //0x06 -> cursor++

void LCD_INIT(int);
void LCD_CMD(uint8_t, uint8_t, int);
void write_char_LCD(uint8_t, uint8_t, int);
void write_string_LCD(char word[], uint8_t, int);
void clear_LCD(int);
void line_clear_LCD(int, int);
void home_LCD(int);

#endif /* LCD_H_ */
