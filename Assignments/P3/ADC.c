/*
 * UART.c
 * Encapsulates ADC management
 * getIntFlag_ADC, clrIntFlag_ADC, calcVolt_ADC, and sendVolt_ADC intended for external use
 * ADC_INIT must be run prior to the use of this library
 *
 * Date: May 11, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "UART.h"
#include "ADC.h"

int anIn;               //Holds sampled number
int dataReady_FLG = 0;  //External sampled data ready flag

void ADC_INIT()
{
    P5 -> SEL1 |= BIT4;   //Sets P5.4 as ADC voltage input
    P5 -> SEL0 |= BIT4;

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);   //Connects NVIC to ADC ISR handler

    //Sets sampling time, S&H=16, ADC14 on
    ADC14 -> CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14 -> CTL1 = ADC14_CTL1_RES_3;           //Sets sampling timer, 14-bit conversion
    ADC14 -> MCTL[0] |= ADC14_MCTLN_INCH_1;     //Sets input select to A1 and VRef=AVCC=3v3
    ADC14 -> IER0 |= ADC14_IER0_IE0;            //Enables ADC data ready interrupt
    ADC14 -> CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}

int getIntFlag_ADC()
{
    return dataReady_FLG;
}

int getAnData_ADC()
{
    return anIn;
}

void clrIntFlag_ADC()
{
    dataReady_FLG = 0;  //Clears sampled data ready flag
    ADC14 -> CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;    //Takes next sample
}

//Calculates voltage corresponding to the passed ADC number
//Returns 3 digit BCD voltage value; 1s 10ths 100ths
int calcVolt_ADC(int extAN)
{
    //N = Mapped digital number; M = Resolution Vin+ = Input voltage
    //Vr+ = Upper ref voltage; Vr- = Lower ref voltage
    //N = (2^M)*(Vin+ - Vr-)/(Vr+ - Vr-), 1LSB = (Vr+ - Vr-)/(2^M)
    //Vin+ = N * Vr+/(2^M) - (1+N/(2^M))Vr-
    int anCal = extAN;

    //Calibrates the ADC depending on the voltage level
    //Calibration values found iteratively
    if (extAN < 2500) {          //0V - 0.5V calibration
        anCal = extAN + 30;
    }
    else if (extAN < 5000) {     //0.5V - 1V calibration
        anCal = extAN + 50;
    }
    else if (extAN < 7500) {    //1V - 1.5V calibration
        anCal = extAN + 75;
    }
    else if (extAN < 10000) {    //1.5V - 2V calibration
        anCal = extAN + 90;
    }
    else if (extAN < 10000) {    //2V - 2.5V calibration
        anCal = extAN + 100;
    }
    else if (extAN < 15000) {    //2.5V - 3V calibration
        anCal = extAN + 100;
    }
    else {
        anCal = extAN + 125;
    }

    //Calculates voltage digits separately utilizing integer rounding
    int ones    = (anCal * VRef) / (RES_14_MAX * 10);
    int tenths  = ((anCal * VRef) / RES_14_MAX) - (10 * ones);
    int hunths  = ((anCal * VRef * 10) / RES_14_MAX) - ((10 * tenths) + (100 * ones));
    int voltage = ((ones & MASK_LOW_32) << 8) | ((tenths & MASK_LOW_32) << 4) |
                   (hunths & MASK_LOW_32);
    return voltage; //Returns BCD voltage
}

//Sends the calculated BCD voltage to the UART TX line
void sendVolt_ADC(int volt)
{
    uint8_t ones = ((volt & 0xF00) >> 8) | 0x30; //Masks, shifts down, and converts BCD to ASCII
    int tenths = ((volt & 0x0F0) >> 4) | 0x30;
    int hunths = (volt & 0x00F) | 0x30;
    sendByte_UART(ones);                         //Sends out each character
    sendByte_UART(DEC_PNT);
    sendByte_UART(tenths);
    sendByte_UART(hunths);
    sendByte_UART(SPACE);
}

//ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    anIn = ADC14->MEM[0];   //Takes in sampled number
    dataReady_FLG = 1;      //Sets data ready flag for external use
}
