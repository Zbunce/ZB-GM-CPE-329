/**
 * main.c
 *
 * Sets up SPI and generates specified wave
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

//TIE LDAC LOW

//P1.5 SCLK
//P1.6 MOSI
//P1.7 MISO

#include "msp.h"
#include <stdint.h>
#include "set_DCO.h"
#include "delays.h"
#include "SPI.h"
#include "DAC.h"

int main(void) {
    int CLK = 120;
    set_DCO(CLK);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    SPI_INIT();                                 //Initializes SPI comms

    while(1) {
        makeWave(square, 2, 1, 50, CLK);        //Generates the specified wave
    }
}
