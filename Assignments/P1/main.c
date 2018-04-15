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

//ASCII hex values for keypad characters
#define K_1     0x31
#define K_2     0x32
#define K_3     0x33

#define K_4     0x34
#define K_5     0x35
#define K_6     0x36

#define K_7     0x37
#define K_8     0x38
#define K_9     0x39

#define K_Ast   0x2A
#define K_0     0x30
#define K_Pnd   0x23

#define K_NP    0x10

void delay_ms(int, int);
void set_DCO(int);
void LCD_INIT(int);
void write_char_LCD(uint8_t, uint8_t, int);
void write_string_LCD(char word[], uint8_t, int);
void clear_LCD(int);
void KEYPAD_INIT();
uint8_t chk_Keypad();

uint8_t chk_Key();
int chk_Password(int);
void LCD_Locked(int);
int LCD_Unlocked(int);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 480;
    int lock = LOCKED;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();

    while(1)
    {
        delay_ms(200, CLK);
        clear_LCD(CLK);
        while(lock == LOCKED)
        {
            LCD_Locked(CLK);
            lock = chk_Password(CLK);
            clear_LCD(CLK);
        }

        while(lock == UNLOCKED)
        {
            lock = LCD_Unlocked(CLK);

        }

    }
}

int LCD_Unlocked(int CLK)
{
    int i;
    uint8_t key;
    char word[] = "HELLO WORLD";
    for(i = 0x00; i <= 0x4F; i++) {
        write_char_LCD(K_NP, i-1, CLK);
        write_string_LCD(word, i, CLK);
        delay_ms(200, CLK);
        key = chk_Keypad();
        if (key == K_Pnd) {
            return LOCKED;
        }
        if ((i > 0x0F) && (i < 0x40)) {
            i = 0x40;
        }
        else if(i == 0x4F) {
            write_char_LCD(K_NP, i, CLK);
            i = 0x00;
        }
    }
    return UNLOCKED;
}

//
int chk_Password(int CLK)
{
    uint8_t key1;
    uint8_t key2;
    uint8_t key3;
    uint8_t key4;
    uint8_t addr = 0x40;

    key1 = chk_Key();
    write_char_LCD(key1, addr, CLK);
    addr++;
    if (key1 == K_Ast) {
        clear_LCD(CLK);
        return LOCKED;
    }
    delay_ms(500, CLK);

    key2 = chk_Key();
    write_char_LCD(key2, addr, CLK);
    addr++;
    if (key2 == K_Ast) {
        clear_LCD(CLK);
        return LOCKED;
    }
    delay_ms(500, CLK);

    key3 = chk_Key();
    write_char_LCD(key3, addr, CLK);
    addr++;
    if (key3 == K_Ast) {
        clear_LCD(CLK);
        return LOCKED;
    }
    delay_ms(500, CLK);

    key4 = chk_Key();
    write_char_LCD(key4, addr, CLK);
    addr++;
    if (key4 == K_Ast) {
        clear_LCD(CLK);
        return LOCKED;
    }
    delay_ms(500, CLK);

    if ((key1 == combo1) && (key2 == combo2) && (key3 == combo3) && (key4 == combo4)) {
        return UNLOCKED;
    }
    else {
        return LOCKED;
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
    write_string_LCD(word, addr, CLK);

    //Sets cursor to bottom row
    letter = 0x10;
    addr   = 0x3F;
    write_char_LCD(letter, addr, CLK);
}
