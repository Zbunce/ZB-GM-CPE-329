/**
 * main.c
 *
 * Locks the LCD until the correct sequence of keys is pressed
 * Once the correct sequence is pressed, displays "HELLO WORLD" unlock message
 * Allows for the previously entered keys to be cleared with "*"
 * Contains a privacy mode in which key presses are not printed
 * Privacy mode is both entered and exited with "#*"
 * Clearing is also enabled while in privacy mode
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

//Mode indicator values
#define NORMAL      1       //NORMAL = Non-Private LOCKED
#define PRIVATE     3       //Privacy enter/exit
#define PRIVLOCK    0xFE    //~LOCKED
#define CLEAR       7       //Entry Clear

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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog timer

    int CLK = 480;                              //48 MHz
    uint8_t lock = LOCKED;                      //Locks the display
    set_DCO(CLK);                               //Sets clock to 48MHz
    LCD_INIT(CLK);                              //Initializes LCD
    KEYPAD_INIT();                              //Initializes keypad

    LCD_Locked(CLK);                            //Sets up the lock screen

    while(1)
    {
        while((lock == LOCKED) || (lock == PRIVLOCK))
        {
            line_clear_LCD(BOTTOM, CLK);    //Clears the password entry line
            lock = chk_Password(lock, CLK); //Checks the next 4 key presses
            line_clear_LCD(BOTTOM, CLK);    //Clears the password entry line
        }

        while(lock == UNLOCKED)
        {
            lock = LCD_Unlocked(CLK);       //Scrolls the unlock message and waits for lock
            delay_ms(200, CLK);             //Waits to avoid holding key press
        }

    }
}

//Checks to see if an alternate modes were activated
//This includes privacy enter, privacy exit, and entry clear
int chk_Mode(uint8_t prevKey, uint8_t curKey, uint8_t mode, int CLK)
{
    if (curKey == K_Ast) {
        if (prevKey == K_Pnd) {
            //Checks to see if privacy mode is already enabled
            if (mode == PRIVLOCK) {
                clear_LCD(CLK);
                LCD_Locked(CLK);                            //Leaves privacy mode
                return PRIVATE;
            }
            else {
                clear_LCD(CLK);
                write_string_LCD("PRIVATE MODE", 0, CLK);   //Enters privacy mode
                return PRIVATE;
            }

        }
        line_clear_LCD(BOTTOM, CLK);                        //Clears previous entries
        return CLEAR;
    }
    return NORMAL;
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

    key1 = chk_Key();   //Waits for first key entry
    newMode = chk_Mode(K_NP, key1, prevMode, CLK);  //Checks for alternate mode entry
    if (newMode == PRIVATE) {
        delay_ms(200, CLK);
        return ~prevMode;   //Enters or exits privacy mode
    }
    else if (newMode == CLEAR) {
        return prevMode;    //Resets password entry
    }

    //Prints characters entered to LCD if in non-privacy mode
    if (prevMode == NORMAL) {
        write_char_LCD(key1, addr, CLK);
        addr++;
    }
    delay_ms(300, CLK);

    //Repeats process for second key press
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

    //Repeats process for third key press
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

    //Repeats process for fourth key press
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

    //Compares passcode entered against correct combo stored in outside header file
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

//Checks for a key press and returns the ASCII character pressed
uint8_t chk_Key()
{
    uint8_t key = 0x10;
    while(key == 0x10)
    {
        key = chk_Keypad(); //Waits until key is pressed
    }
    return key;
}

//Sets up non-privacy mode lock screen message
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

//Scrolls unlock message and waits for lock input
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

        //Locks LCD if "#" is pressed
        if (key == K_Pnd) {
            LCD_Locked(CLK);
            return LOCKED;
        }
        //Handles edge cases
        if(i == 0x00) {
            i = 0x4F;
        }
        else if ((i > 0x0F) && (i < 0x40)) {
            i = 0x0F;
        }
    }
    return UNLOCKED;
}
