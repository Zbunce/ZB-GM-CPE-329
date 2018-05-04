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

int main(void) {
    int CLK = 120;
    set_DCO(CLK);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    KEYPAD_INIT();
    SPI_INIT();                                 //Initializes SPI comms

    while(1) {
        makeWave(square, 2, 1, 50, CLK);        //Generates the specified wave
    }
}

void FG_INIT()
{
    int[410] sinVal;
    int i;
    for(i = 0; i < 410, i++) {
        pi_10000 = 31415
        x =
        sinVal[i] = sin(x);
    }
}

#define F_100Hz 100
#define F_200Hz 200
#define F_300Hz 300
#define F_400Hz 400
#define F_500Hz 500

void freqHandler_FG(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    int i=0;
    switch (freq)
    {
    case F_100Hz:

        if caseInt == 1
        {
          break;
        }
        else
        {
            if intFlag == 1
            {
                i++;
                point = sinVal[i];
            }
            writeDac(point);
        }
        break;

    case F_200Hz:

        if caseInt == 1
        {
          break;
        }
        else
        {
            if intFlag == 1
            {
                i += 2;
                point = sinVal[i];
            }
            writeDac(point);
        }
        break;

    case F_300Hz:
        if caseInt == 1
        {
          break;
        }
        else
        {
            if intFlag == 1
            {
                i += 3;
                point = sinVal[i];
            }
            writeDac(point);
        }
        break;

    case F_400Hz:
        if caseInt == 1
        {
          break;
        }
        else
        {
            if intFlag == 1
            {
                i += 4;
                point = sinVal[i];
            }
            writeDac(point);
        }
        break;

    case F_500Hz:
        if caseInt == 1
        {
          break;
        }
        else
        {
            if intFlag == 1
            {
                i += 5;
                point = sinVal[i];
            }
            writeDac(point);
        }
        break;

    }

}
