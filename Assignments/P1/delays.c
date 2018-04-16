/*
 * delays.c
 * Code file for both the ms and us delay functions.
 * Utilizes __delay__cycles
 * Divide us by 2 cause ?
 * 
 * Date: April 9, 2018
 * Authors: Zach Bunce, Garret Maxon
 */

#include "msp.h"

#define F_1p5_MeHz  15      //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30      //MeHz labels are used to indicate this
#define F_6_MeHz    60      //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

//Takes in desired time delay in ms and clock frequency in MeHz
//Accurate to less than 1%
void delay_ms(int time_ms, int freq_MeHz)
{
    int i;
    //Unit Conversion: MeHz * ms = 10^5 * 10^-3 = 10^2 = 100
    switch (freq_MeHz)
    {
    case F_1p5_MeHz:
        //Accurate to 0.1%
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(1500);
        }
        break;
    case F_3_MeHz:
        //Accurate to 0.4%
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(3000);
        }
        break;
    case F_6_MeHz:
        //Accurate to 0.6%
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(6000);
        }
        break;
    case F_12_MeHz:
        //Accurate to 0.7%
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(12000);
        }
        break;
    case F_24_MeHz:
        //Accurate to 0.6%
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(24000);
        }
        break;
    case F_48_MeHz:
        //Accurate to 0.4%
        //Div by 2 cause ?
        for (i = time_ms; i > 0; i--) {
            __delay_cycles(24000);
        }
        break;
    default:
        break;
    }
}

//Takes in desired time delay in us and clock frequency in MeHz
//1.5, 3, 6, 12 MHz NOT TUNED
void delay_us(int time_us, int freq_MeHz)
{
    int i;
    int j;
    int z;
    int time_fix;
    //Unit Conversion: MeHz * us = 10^5 * 10^-6 = 10^-1 = 0.1; Accounted for in decrement
    switch (freq_MeHz)
    {
    case F_1p5_MeHz:
        time_fix = time_us / 2;
        for (i = time_fix; i > 0; i--) {
            for (j = 10; j > 0; j--) {
                __delay_cycles(15);
            }
        }
        break;
    case F_3_MeHz:
        time_fix = time_us / 2;
        for (i = time_fix; i > 0; i--) {
            __delay_cycles(3);
        }
        break;
    case F_6_MeHz:
        time_fix = time_us / 2;
        for (i = time_fix; i > 0; i--) {
            __delay_cycles(6);
        }
        break;
    case F_12_MeHz:
        time_fix = time_us / 2;
        for (i = time_fix; i > 0; i--) {
            __delay_cycles(12);
        }
        break;
    case F_24_MeHz:
        //Within +1us 26 < t < 100; Accurate within 1% up to 1000 us
        time_fix = (time_us - 1) / 2;
        for (i = time_fix; i > 0; i--) {
            __delay_cycles(24);
            z++;
            z++;
            z++;
        }
        break;
    case F_48_MeHz:
        //Within +1us 25 < t < 100; Accurate within 2% up to 1000 us
        time_fix = time_us / 2;
        for (i = time_fix; i > 0; i--) {
            __delay_cycles(48);
            z++;
            z++;
            z++;
            z++;
        }
        break;
    default:
        break;
    }
}
