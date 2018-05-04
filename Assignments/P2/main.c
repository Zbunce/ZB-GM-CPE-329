/**
 * main.c
 *
 * Sets up SPI and generates specified wave
 *
 * Date: April 2 2018
 * Authors: Zach Bunce, Garrett Maxon
 */
//Constant incr delay speccd
//TIE LDAC LOW

//P1.5 SCLK
//P1.6 MOSI
//P1.7 MISO

#include "msp.h"
#include <stdint.h>
#include <math.h>
#include "set_DCO.h"
#include "delays.h"
#include "SPI.h"
#include "DAC.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    uint8_t lock = LOCKED;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();
    SPI_INIT();                                 //Initializes SPI comms

    int waveT = 1; // default values
    int frequency = 100;
    int  duty = 50;  //not sure what this links to

    while(1)
    {

        switch (chk_Key())
        {
        case K_1:
            frequency = 100;
            break;
        case K_2:
            frequency = 200;
            break;
        case K_3:
            frequency = 300;
            break;
        case K_4:
            frequency = 400;
            break;
        case K_5:
            frequency = 500;
            break;
        case K_7:
            waveT = 1;
            break;
        case K_8:
            waveT = 3;
            break;
        case K_9:
            waveT = 4;
            break;
        case K_Ast:
            duty = duty - 10;
            break;
        case K_Pnd:
            duty = duty + 10;
            break;
        case K_0:
            duty = 50;
            break;
        }

        if (waveT = 1)
        {
            word[] = "Type: " + waveT + " DC:" + duty + "%" + "Freq: " + freq + "Hz";
        }
        else
        {
            word[] = "Type: " + waveT + "       " + "Freq: " + freq + "Hz";
        }

        write_string_LCD(word, 0, CLK);

        incFlag = chk_Flag();
        if (incFlag == 1) {
            makeWave(waveT, frequency, DC, CLK)
        }
        else {

        }




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




