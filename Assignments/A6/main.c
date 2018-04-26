#include "msp.h"
#include <stdint.h>
#include "DAC.h"
#include "SPI.h"
#include "set_DCO.h"
#include "delays.h"

#define square      1
#define triangle    2
#define sine        3
#define Vref        33

void delay_ms(int, int);
void delay_us(int, int);
void set_DCO(int);
void write_DAC(uint16_t);



int main (void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    set_DCO(CLK);
}

void makeDC (int volt)
{
    uint16_t DN = (((4096*volt)/(Vref))*10);   //generating the DAC input code from given voltage
    write_DAC.(DN);
}

void makeWave(int waveType, int pp , int offset, int period)
{
    uint32_t i;

    if (waveType == 1) //square
    {
        int top     = pp + offset;      //top voltage
        int bottom  = pp - offset;   //bottom voltage
        int width   = period/2;       // width of pulse

        while(1)
        {
        makeDC(top);                //makes top of the square wave
        delay_ms(width , CLK);      //delays the width of the pulse
        makeDC(bottom);             // makes the bottom of the pulse .
        delay_ms(width , CLK);      //delays the width of the pulse
        }

    }
    else if (waveType == 2) //triangle
    {

        int top      = pp + offset;      //top voltage
        int bottom   = pp - offset;   //bottom voltage
        int width    = period/2;       // width of pulse
        uint16_t DN_Top     = (((4096*volt)/(Vref))*10);     //finds the DAC input code of the top peak
        uint16_t DN_Bottom  = (((4096*volt)/(Vref))*10); // finds the DAC input code of the bottom peak
        int delay = (width/(DN_Top - DN_Bottom))/1000; //finds how long each "step" needs to be in terms or microseconds to achieve the period

        while(1)
        {
            uint16_t DN_Point = DN_Bottom;
            for (DN_Point < DN_Top)
            {
                write_DAC.(DN_Point);   //writes the voltage "point"
                delay_us(delay, CLK);      // delays a little
                DN_Point++;             // move the point up the slope

            }

            for ( DN_Point > DN_Bottom)
            {
                write_DAC.(DN_Point);   //writes the voltage "point"
                delay_us(30, CLK);      // delays a little
                DN_Point--;             // move the point down the slope
            }
            //back to top
        }


    }
    else if (waveType == 3)
    {
        //not yet
    }

}
