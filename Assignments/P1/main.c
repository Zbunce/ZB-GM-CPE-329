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
#include "combo.h"

#define LOCKED      1
#define UNLOCKED    0

//Operating mode indicators
#define NORMAL      1
#define PRIVATE     3
#define PRIVLOCK    0xFE
#define CLEAR       7

void delay_ms(int, int);
void set_DCO(int);
void LCD_INIT(int);
void write_char_LCD(uint8_t, uint8_t, int);
void write_string_LCD(char word[], uint8_t, int);
void clear_LCD(int);
void line_clear_LCD(int, int);
void KEYPAD_INIT();
uint8_t chk_Keypad();

uint8_t chk_Key();
int chk_Password(uint8_t, int);
void LCD_Locked(int);
int LCD_Unlocked(int);
int chk_Privacy(uint8_t, uint8_t);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 480;
    uint8_t lock = LOCKED;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();

    LCD_Locked(CLK);

    while(1)
    {
        while((lock == LOCKED) || (lock == PRIVLOCK))
        {
            line_clear_LCD(BOTTOM, CLK);
            lock = chk_Password(lock, CLK);
            line_clear_LCD(BOTTOM, CLK);
        }

        while(lock == UNLOCKED)
        {
            lock = LCD_Unlocked(CLK);
            delay_ms(200, CLK);
        }

    }
}

int chk_Mode(uint8_t prevKey, uint8_t curKey, uint8_t mode, int CLK)
{
    if (curKey == K_Ast) {
        if (prevKey == K_Pnd) {
            if (mode == PRIVLOCK) {
                clear_LCD(CLK);
                LCD_Locked(CLK);
                return PRIVATE;
            }
            else {
                clear_LCD(CLK);
                write_string_LCD("PRIVATE MODE", 0, CLK);
                return PRIVATE;
            }

        }

        line_clear_LCD(BOTTOM, CLK);
        return CLEAR;
    }
    return NORMAL;
}

int LCD_Unlocked(int CLK)
{
    int i;
    uint8_t key;
    char word[] = "HELLO WORLD";
    for(i = 0x4F; i >= 0x00; i--) {
        clear_LCD(CLK);
        write_string_LCD(word, i, CLK);
        delay_ms(200, CLK);
        key = chk_Keypad();
        if (key == K_Pnd) {
            LCD_Locked(CLK);
            return LOCKED;
        }

        if(i == 0x00) {
            i = 0x4F;
        }
        else if ((i > 0x0F) && (i < 0x40)) {
            i = 0x0F;
        }
    }
    return UNLOCKED;
}

//
int chk_Password(uint8_t prevMode, int CLK)
{
    uint8_t key1;
    uint8_t key2;
    uint8_t key3;
    uint8_t key4;
    uint8_t addr = 0x40;
    uint8_t newMode;

    key1 = chk_Key();
    newMode = chk_Mode(K_NP, key1, prevMode, CLK);
    if (newMode == PRIVATE) {
        delay_ms(200, CLK);
        return ~prevMode;
    }
    else if (newMode == CLEAR) {
        return prevMode;
    }

    if (prevMode == NORMAL) {
        write_char_LCD(key1, addr, CLK);
        addr++;
    }
    delay_ms(300, CLK);

    key2 = chk_Key();
    newMode = chk_Mode(key1, key2, CLK);
    if (newMode == PRIVATE) {
        delay_ms(200, CLK);
        return ~prevMode;
    }
    else if (newMode == CLEAR) {
        return prevMode;
    }

    if (prevMode == NORMAL) {
        write_char_LCD(key2, addr, CLK);
        addr++;
    }
    delay_ms(300, CLK);

    key3 = chk_Key();
    newMode = chk_Mode(key2, key3, CLK);
    if (newMode == PRIVATE) {
        delay_ms(200, CLK);
        return ~prevMode;
    }
    else if (newMode == CLEAR) {
        return prevMode;
    }

    if (prevMode == NORMAL) {
        write_char_LCD(key3, addr, CLK);
        addr++;
    }
    delay_ms(300, CLK);

    key4 = chk_Key();
    newMode = chk_Mode(key3, key4, CLK);
    if (newMode == PRIVATE) {
        delay_ms(200, CLK);
        return ~prevMode;
    }
    else if (newMode == CLEAR) {
        return prevMode;
    }

    if (prevMode == NORMAL) {
        write_char_LCD(key4, addr, CLK);
        addr++;
    }
    delay_ms(300, CLK);

    if ((key1 == combo1) && (key2 == combo2) && (key3 == combo3) && (key4 == combo4)) {
        return UNLOCKED;
    }
    else {
        line_clear_LCD(BOTTOM, CLK);
        write_string_LCD("INCORRECT CODE", 0x40, CLK);
        delay_ms(500, CLK);
        return prevMode;
    }

}

uint8_t chk_Key()
{
    uint8_t key = 0x10;
    while(key == 0x10)
    {
        key = chk_Keypad();
    }
    return key;
}

void LCD_Locked(int CLK)
{
    uint8_t addr;
    uint8_t letter;
    char word[] = "LOCKED ENTER KEY";
    //Writes lock phrase
    addr = 0x00;
    clear_LCD(CLK);
    write_string_LCD(word, addr, CLK);

    //Sets cursor to bottom row
    letter = 0x10;
    addr   = 0x3F;
    write_char_LCD(letter, addr, CLK);
}
