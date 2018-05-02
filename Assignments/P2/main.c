#include "msp.h"


/**
 * main.c
 */

#include "msp.h"
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "keypad.h"
#include "combo.h"
#include "DAC.h"

#define f100 100
#define f200 200
#define f300 300
#define f400 400
#define f500 500

void delay_ms(int, int);
void set_DCO(int);
void LCD_INIT(int);
void write_char_LCD(uint8_t, uint8_t, int);
void write_string_LCD(char word[], uint8_t, int);
void clear_LCD(int);


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int i=0;
	switch (freq)
	{
	case f100:

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

	case f200:

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

	case f300:
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

	case f400:
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

    case f500:
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