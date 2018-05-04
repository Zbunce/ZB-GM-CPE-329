#include "msp.h"


/**
 * main.c
 */

#include "msp.h"
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "keypad.h"
#include "combo.h"
#include "DAC.h"

#define f100 100
#define f200 200
#define f300 300
#define f400 400
#define f500 500

#define square      1       //Wave type definitions
#define triangle    2
#define sine        3

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

void set_DCO(int);
void LCD_INIT(int);
void write_string_LCD(char word[], uint8_t, int);
void KEYPAD_INIT();
uint8_t chk_Keypad();
uint8_t chk_Key();


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    int CLK = 480;
    uint8_t lock = LOCKED;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT()

    int waveT = 1; // default values
    int frequency = 100;
    int  duty = 500;  //not sure what this links to

    while(1)
    {

        switch (chk_Key())
        {
        case K_1:
            frequency = 100;
        case K_2:
            frequency = 200;
        case K_3:
            frequency = 300;
        case K_4:
            frequency = 400;
        case K_5:
            frequency = 500;
        case K_7:
            waveT = 1;
        case K_8:
            waveT = 3;
        case K_9:
            waveT = 4;
        case K_Ast:
            duty = duty - 10;
        case K_Pnd:
            duty = duty + 10;
        case K_0:
            duty = 500;
        }
        if (waveT = 1)
        {
            word[] = "Duty Cycle: " + duty + "Wave Type: " + waveT;
        }
        else
        {
            word[] = "Frequency: " + frequency + "Wave Type: " + waveT;
        }
        write_string_LCD(word, 1, CLK);
        makeWave(waveT, pp, offset, frequency, CLK)

    // write the wave
    // write the LCD
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
