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
#include <stdint.h>
#include "SPI.h"
#include "DAC.h"
#include "delays.h"

static int z = 0;       //Timer division variable
static int s = 0;       //Sin value index variable
static int t = 0;       //Sawtooth value index variable
static int waveType;    //Wave property variables for ISR
static int DC
static int freq;
static int freq_idx;
static int CLK;
static int timeDiv;
static int time_High;
static int time_Low;

static uint8_t sqw_ST;  //Square wave state variable for ISR

static int UD;          //Triangle wave ISR variables
static int16_t DN_Point;
static int incDiv;


static int[200] sinVal;

void FG_INIT()
{
    int i;
    int x;
    int divs = 200;
    int shift = 1000000;
    for(i = 0; i < divs, i++) {
        x = (2 * i * shift)/divs * M_PI;
        sinVal[i] = sin(x);
    }
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

void makeWave(int waveT, int frequency, int duty, int clock)
{
    waveType = waveT;   //Links outside specified parameters to ISR globals
    freq = frequency;
    freq_idx = frequency / 100;
    DC = duty;
    CLK = clock;

    int delCyc

    //int16_t DN_Top     = HIGH;  //Calculates the DAC input for the peak
    //int16_t DN_Bottom  = LOW;  //Calculates the DAC input for the trough

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC

    //int top = Vpp + Voff;                       //Calculates peak voltage
    //int bot = Voff;                             //Calculates trough voltage

    if (waveType == square)                     //Square wave
    {
        del_High  = (((CLK * 100000) / freq)* DC)/100;  //Calculates frequency division needed
        time_High  = del_High / (60000*2);           //Maps this division to usable clock increments
		del_Low = (((CLK * 100000) / freq)* (1-DC))/100;
		time_Low = del_Low / (60000*2); 
        write_DAC(sqw_ST);
        TIMER_A0->CCR[0] = 60000;               //Initializes first high time count
        //while(1);                               //Allows interrupt to control square wave generation
    }
    /*else if (waveType == triangle)              //Triangle wave
    {
        DN_Point = (((4096*Voff)/(Vref))*10);   //Sets the initial DAC input to the trough
        incDiv = ((CLK * 1000000) / (2 * (DN_Top - DN_Bottom) * freq)); //Calculates the counter amount needed
        TIMER_A0->CCR[0] = incDiv;              //Initializes first increment count
        while(1);                               //Allows interrupt to control triangle wave generation
    }
	*/
    else if (waveType == sine)
    {

    }
	else if (waveType == sawtooth)
	{
		
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
        if (z == timeDiv) {
            sqw_ST = ~sqw_ST;                   //Inverts square wave level
            if (sqw_ST == HIGH) {
                timeDiv = time_High;           //Sets output voltage to high value
            }
            else if (sqw_ST == LOW) {
                timeDiv = time_Low;                  //Sets output voltage to low value
            }
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
        DN_Point = sinVal[s+freq_idx];
        TIMER_A0->CCR[0] += sinDel;              //Adds next offset to TACCR0
    }
    else if(waveType == sawtooth) {
        DN_Point = sawVal[t+freq_idx];
        TIMER_A0->CCR[0] += sawDel;              //Adds next offset to TACCR0
    }
}
