/*
 * LCD.h
 * Header file for LCD control.
 *
 * Created on: April 10, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef LCD_H_
#define LCD_H_

void LCD_INIT(int);
void LCD_CMD(unsigned char, unsigned char, int);
void write_char_LCD(unsigned char, unsigned char, int);
void clear_LCD(int);
void home_LCD(int);

#endif /* LCD_H_ */
