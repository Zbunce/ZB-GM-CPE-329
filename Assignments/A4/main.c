/**
 * main.c
 *
 * Links the keypad to the LCD, displaying key pressed
 * Consecutive key presses overwrite previous entry
 *
 * Date: April 13 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "keypad.h"

void LCD_INIT(int);
void write_char_LCD(uint8_t, uint8_t, int);
void clear_LCD(int);

void KEYPAD_INIT();
uint8_t chk_Keypad();

void delay_ms(int, int);
void set_DCO(int);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 480;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();

    uint8_t key = 0x10;
    uint8_t addr = 0x00;
    while(1) {
        while(key == 0x10) {
            key = chk_Keypad();
        }

        write_char_LCD(key, addr, CLK);
        home_LCD(CLK);
        delay_ms(500,CLK);
        key = 0x10;
    }
}
