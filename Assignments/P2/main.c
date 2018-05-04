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
#include <string.h>
#include "set_DCO.h"
#include "delays.h"
#include "keypad.h"
#include "SPI.h"
#include "DAC.h"
#include "LCD.h"

uint8_t chk_Key();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    char labels[] = "Type:     DC:  %Freq:    Hz";
    int incFlag = 0;
    char freq_Xfr[3] = "100";
    char waveType[3] = "SQR";
    char duty_Xfr[2] = "50";

    uint8_t key = K_NP;
    char waveT = 1; // default values
    int frequency = 100;
    int  duty = 50;  //not sure what this links to

    int CLK = 120;
    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();
    SPI_INIT();                                 //Initializes SPI comms

    write_string_LCD(labels, 0, CLK);

    FG_INIT();

    while(1)
    {
        switch (key)
        {
        case K_1:
            frequency = 100;
            strcpy(freq_Xfr, "100");
            break;
        case K_2:
            frequency = 200;
            strcpy(freq_Xfr, "200");
            break;
        case K_3:
            frequency = 300;
            strcpy(freq_Xfr, "300");
            break;
        case K_4:
            frequency = 400;
            strcpy(freq_Xfr, "400");
            break;
        case K_5:
            frequency = 500;
            strcpy(freq_Xfr, "500");
            break;
        case K_7:
            waveT = 1;
            duty = 50;
            strcpy(duty_Xfr, "50");
            strcpy(waveType, "SQR");
            break;
        case K_8:
            waveT = 3;
            strcpy(waveType, "SIN");
            break;
        case K_9:
            waveT = 4;
            strcpy(waveType, "SAW");
            break;
        case K_Ast:
            duty = duty - 10;
            switch(duty) {
            case 10:
                strcpy(duty_Xfr, "10");
                break;
            case 20:
                strcpy(duty_Xfr, "20");
                break;
            case 30:
                strcpy(duty_Xfr, "30");
                break;
            case 40:
                strcpy(duty_Xfr, "40");
                break;
            case 50:
                strcpy(duty_Xfr, "50");
                break;
            case 60:
                strcpy(duty_Xfr, "60");
                break;
            case 70:
                strcpy(duty_Xfr, "70");
                break;
            case 80:
                strcpy(duty_Xfr, "80");
                break;
            case 90:
                strcpy(duty_Xfr, "90");
                break;
            }
            break;
        case K_Pnd:
            duty = duty + 10;
            switch(duty) {
            case 10:
                strcpy(duty_Xfr, "10");
                break;
            case 20:
                strcpy(duty_Xfr, "20");
                break;
            case 30:
                strcpy(duty_Xfr, "30");
                break;
            case 40:
                strcpy(duty_Xfr, "40");
                break;
            case 50:
                strcpy(duty_Xfr, "50");
                break;
            case 60:
                strcpy(duty_Xfr, "60");
                break;
            case 70:
                strcpy(duty_Xfr, "70");
                break;
            case 80:
                strcpy(duty_Xfr, "80");
                break;
            case 90:
                strcpy(duty_Xfr, "90");
                break;
            }
            break;
        case K_0:
            duty = 50;
            strcpy(duty_Xfr, "50");
            break;
        }

        if (waveT == 1) {
            write_string_LCD(waveType, 0x06, CLK);
            write_string_LCD(duty_Xfr, 0x0D, CLK);
            write_string_LCD(freq_Xfr, 0x46, CLK);
        }
        else {
            strcpy(duty_Xfr, "  ");
            write_string_LCD(waveType, 0x06, CLK);
            write_string_LCD(duty_Xfr, 0x0D, CLK);
            write_string_LCD(freq_Xfr, 0x46, CLK);
        }

        key = K_NP;
        delay_ms(200, CLK);

        while(key == K_NP) {
            incFlag = chk_FGFlag();
            if (incFlag == 1) {
                makeWave(waveT, frequency, duty, CLK);
            }
            key = get_Key();
        }
    }
}




