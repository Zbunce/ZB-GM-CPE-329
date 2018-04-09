/*
 *  A2 Main
 *  Implements various led pulses.
 *
 *  Date: April 4, 2018
 *  Authors: Zach Bunce, Garrett Maxon
 */
//Toggles BIT0
//Indefinitely counts to 2k & then flips BIT0

#include "msp.h"
#include "delays.h"
#include "set_DCO.h"

#define F_1p5_MeHz  15      //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30      //MeHz labels are used to indicate this
#define F_6_MeHz    60      //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

void set_DCO(int);
void delay_ms(int, int);
void delay_us(int, int);

int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;    // Stop WDT

    // P1.0 set as GPIO
    P1->SEL0 &= ~BIT0;              //Clear bit 0 of the P1->SEL0 register
    P1->SEL1 &= ~BIT0;              //Clear bit 0 of the P1->SEL1 register

    P1->DIR |= BIT0;                //P1.0 set as output
    set_DCO(F_24_MeHz);        //Sets the DCO to 1.5 MHz
    //Creates three 1000 ms pulses using 1.5 MHz, 6 MHz, and 24 MHz
    while (1)                       //Continuous loop
    {

        P1->OUT |= BIT0;            //Turn on P1.0 LED
        delay_us(30, F_24_MeHz); //Waits for 1000 ms
        P1->OUT &= ~BIT0;            //Turn off P1.0 LED
        delay_us(30, F_24_MeHz);  //Wait a half pulse
    }
}
