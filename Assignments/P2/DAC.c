/**
 * DAC.c
 *
 * Contains functions for running the DAC
 * Allows for DC voltages and various wave types to be automatically produced
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include <math.h>
#include <stdint.h>
#include "SPI.h"
#include "DAC.h"
#include "delays.h"

static int z = 0;       //Timer division variable
static int s = 0;       //Sin value index variable
static int t = 0;       //Sawtooth value index variable
static int waveType;    //Wave property variables for ISR
static int freq_idx;

static int sinVal_DN[];
static int sawVal_DN[];
static int sinDel;
static int sawDel;
static int sqrDiv;

static uint8_t sqr_ST;  //Square wave state variable for ISR

static int16_t DN_Point;

static int intFlag = 0;


void FG_INIT()
{
    int i;
    uint16_t sin_DN;
    uint16_t saw_DN;
    float sinVolt;
    float x;
    int divs = 200;
    //int shift = 10000;
    for(i = 0; i <= divs; i++) {
        x = (2 * i)/divs * M_PI;
        sinVolt = sin(x);
        sin_DN = ((4096 * sinVolt * 10) / Vref);           //Sets the sin DAC
        saw_DN = ((4096 * i) / divs);           //Sets the sawtooth DAC
        sinVal_DN[i] = sin_DN;
        sawVal_DN[i] = saw_DN;
    }

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC
    TIMER_A0->CCR[0] = 60000;               //Initializes first high time count
}

void makeDC(int volt)
{
    uint16_t DN = (((4096*volt)/(Vref))*10);    //Maps the 12-bit DAC value for the desired output voltage
    write_DAC(DN);                              //Writes value to the DAC
}

/*More general makeWav; further simplified
void makeWave(int waveT, int pp, int offset, int frequency, int clock)
{
    waveType = waveT;   //Links outside specified parameters to ISR globals
    Vpp = pp;
    Voff = offset;
    freq = frequency;
    CLK = clock;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC

    int top = Vpp + Voff;                       //Calculates peak voltage
    int bot = Voff;                             //Calculates trough voltage

    if (waveType == square)                     //Square wave
    {
        TIMER_A0->CCR[0] = 60000;               //Initializes first high time count
        while(1);                               //Allows interrupt to control square wave generation
    }
    else if (waveType == triangle)              //Triangle wave
    {
        DN_Point = (((4096*Voff)/(Vref))*10);           //Sets the initial DAC input to the trough
        int16_t DN_Top     = (((4096*top)/(Vref))*10);  //Calculates the DAC input for the peak
        int16_t DN_Bottom  = (((4096*bot)/(Vref))*10);  //Calculates the DAC input for the trough
        incDiv = ((CLK * 1000000) / (2 * (DN_Top - DN_Bottom) * freq)); //Calculates the counter amount needed
        TIMER_A0->CCR[0] = incDiv;              //Initializes first increment count
        while(1);                               //Allows interrupt to control triangle wave generation
    }
}*/

int chk_FGFlag() {
    return intFlag;
}

void clr_FGFlag() {
    intFlag = 0;
}

void makeWave(int waveT, int freq, int DC, int CLK)
{
    waveType = waveT;   //Links outside specified parameters to ISR globals
    freq_idx = freq / 100;

    int delCyc_H;
    int delCyc_L;
    int sqrDiv_H;
    int sqrDiv_L;

    if (waveType == square)                     //Square wave
    {
        delCyc_H  = (CLK * 100000 * DC) / (freq * 100);    //Calculates frequency division needed
        sqrDiv_H = delCyc_H / (60000*2);       //Maps this division to usable clock increments
        delCyc_L = (CLK * 100000 * (100 - DC)) / (freq * 100); //Calculates frequency division needed
        sqrDiv_L = delCyc_L / (60000*2);       //Maps this division to usable clock increments
        write_DAC(sqr_ST);
        if (sqr_ST == HIGH) {
            sqrDiv = sqrDiv_H;
        }
        else if (sqr_ST == LOW) {
            sqrDiv = sqrDiv_L;
        }
        //while(1);                               //Allows interrupt to control square wave generation
    }
//    else if (waveType == triangle)              //Triangle wave
//    {
//        DN_Point = (((4096*Voff)/(Vref))*10);   //Sets the initial DAC input to the trough
//        incDiv = ((CLK * 1000000) / (2 * (DN_Top - DN_Bottom) * freq)); //Calculates the counter amount needed
//        TIMER_A0->CCR[0] = incDiv;              //Initializes first increment count
//        //while(1);                               //Allows interrupt to control triangle wave generation
//    }
//    else if (waveType == sine)
//    {
//        write_DAC(DN_Point);
//    }
//    else if (waveType == sawtooth)
//    {
//        write_DAC(DN_Point);
//    }
    else {
        write_DAC(DN_Point);
    }

}


void write_DAC(uint16_t data)
{
    uint8_t up_Byte;
    uint8_t low_Byte;

    //Shifts the upper nibble into the lower, masks the 4 data bits, appends control bits
    up_Byte  =  ((data >> 8) & 0x0F) | (GAIN1 | SDOFF);
    low_Byte =   (data & 0x00FF);       //Masks bottom 8 data bits

    P5 -> OUT &= ~BIT5;                 //Lowers chip select
    sendByte_SPI(up_Byte);              //Transmits upper byte on SPI line
    sendByte_SPI(low_Byte);             //Transmits lower byte on SPI line
    P5 -> OUT |= BIT5;                  //Sets chip select
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag

    if (waveType == square) {
        if (z == sqrDiv) {
            sqr_ST = ~sqr_ST;                   //Inverts square wave level
            DN_Point = sqr_ST;
            z = 0;                              //Clears ISR entry counter
        }
        z++;                                    //Increments ISR entry counter
        TIMER_A0->CCR[0] += 60000;              //Adds next offset to TACCR0
    }
    /*else if (waveType == triangle) {
        int16_t DN_Top     = (((4096*(Vpp + Voff))/(Vref))*10); //Calculates the DAC input for the peak
        int16_t DN_Bottom  = (((4096*Voff)/(Vref))*10);         //Calculates the DAC input for the trough

        if (DN_Point >= DN_Top) {
            UD = -10;               //Sets the wave to decrement
            DN_Point = DN_Top;      //Ensures output is at peak value
        }
        else if (DN_Point <= DN_Bottom) {
            UD = 10;                //Sets the wave to increment
            DN_Point = DN_Bottom;   //Ensures output is at trough value
        }
        write_DAC(DN_Point);        //Sends the DAC value to the DAC
        DN_Point += UD;             //Calculates increment or decrement for DAC value
        TIMER_A0->CCR[0] += incDiv; //Adds 5ms offset to TACCR0*/
    else if(waveType == sine) {
        s += freq_idx;
        DN_Point = sinVal_DN[s];
        intFlag = 1;
        TIMER_A0->CCR[0] += sinDel;              //Adds next offset to TACCR0
    }
    else if(waveType == sawtooth) {
        t += freq_idx;
        DN_Point = sawVal_DN[t];
        intFlag = 1;
        TIMER_A0->CCR[0] += sawDel;              //Adds next offset to TACCR0
    }
}
