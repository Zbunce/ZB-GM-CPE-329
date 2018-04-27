/**
 * DAC.c
 *
 * DAC management
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include <stdint.h>
#include "SPI.h"
#include "DAC.h"
#include "delays.h"

void sendByte_SPI(uint8_t);
void SPI_INIT();
void write_DAC(uint16_t);

void delay_ms(int, int);
void delay_us(int, int);

void makeDC(int volt)
{
    uint16_t DN = (((4096*volt)/(Vref))*10);   //generating the DAC input code from given voltage
    write_DAC(DN);
}

void makeWave(int waveType, int Vpp, int offset, int period, int CLK)
{
    uint32_t i;

    if (waveType == square) //square
    {
        int top     = Vpp + offset;      //top voltage
        int bottom  = Vpp - offset;   //bottom voltage
        int width   = period/2;       // width of pulse

        while(1)
        {
            makeDC(top);                //makes top of the square wave
            delay_ms(width , CLK);      //delays the width of the pulse
            makeDC(bottom);             // makes the bottom of the pulse .
            delay_ms(width , CLK);      //delays the width of the pulse
        }

    }
    else if (waveType == triangle)      //triangle
    {

        int top      = Vpp + offset;    //top voltage
        int bottom   = Vpp - offset;    //bottom voltage
        int width    = period/2;        // width of pulse
        uint16_t DN_Top     = (((4096*top)/(Vref))*10);     //finds the DAC input code of the top peak
        uint16_t DN_Bottom  = (((4096*bottom)/(Vref))*10); // finds the DAC input code of the bottom peak
        int delay = (width/(DN_Top - DN_Bottom))/1000; //finds how long each "step" needs to be in terms or microseconds to achieve the period

        while(1)
        {
            uint16_t DN_Point = DN_Bottom;
            while(DN_Point < DN_Top)
            {
                write_DAC(DN_Point);    //writes the voltage "point"
                delay_us(delay, CLK);   // delays a little
                DN_Point++;             // move the point up the slope
            }

            while(DN_Point > DN_Bottom)
            {
                write_DAC(DN_Point);    //writes the voltage "point"
                delay_us(30, CLK);      // delays a little
                DN_Point--;             // move the point down the slope
            }
            //back to top
        }


    }
    else if (waveType == sine)
    {
        //not yet
    }

}

void write_DAC(uint16_t data)
{
    uint8_t up_Byte  =  ((data & 0x0F00) >> 8);
    up_Byte |= (GAIN1 | SDOFF);
    uint8_t low_Byte =   (data & 0x00FF);

    P2 -> DIR |= BIT4;
    P2 -> OUT &= ~BIT4;
    delay_us(50, 120);
    sendByte_SPI(up_Byte);
    delay_us(50, 120);
    sendByte_SPI(low_Byte);
    delay_us(50, 120);
    P2 -> OUT |= BIT4;
}
