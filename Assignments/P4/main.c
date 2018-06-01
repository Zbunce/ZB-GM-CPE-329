#include "msp.h"
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "I2C.h"

static int pageNum;
static int msCount;


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	
	int CLK = 120;
	
	P3->DIR = ~(uint8_t) BIT1;
    P3->OUT = BIT1;
    P3->REN = BIT1;                         // Enable pull-up resistor (P3.1 output high)
	
	P2->DIR = ~(uint8_t) BIT1;
    P2->OUT = BIT1;
    P2->REN = BIT1;   		

	// Enable pull-up resistor (P2.1 output high)
	//changes pages based on two buttons
	//the first button goes to the next page and wraps back to the frist after the third page
	//the second button goes to the previous page and wraps back to page three after the first page
	if (button1 == 0)
	{
		pageNum ++;
		if (pageNum > 3); 
		{
			pageNum = 1;
		}
	}
	if (button2 == 0)
	{
		pageNum +-;
		if (pageNum < 0 ); 
		{
			pageNum = 3;
		}
	}
}

void writeDisp()
{
	//three pages to display information on
	//Velcoity & Acceleration (page 1) - Max Velocity & Acceleration(page 2) - 0-60 timer (page 3)
	
	if (pageNum == 1)
	{
		write_string_LCD("CURR VEL- ", 0x00, CLK);
		write_string_LCD(currVel, 0x09, CLK);
		write_string_LCD("CURR ACCEL- ", 0x00, CLK);
		write_string_LCD(currAccel, 0x0B, CLK);
	}
	else if (pageNum == 2)
	{
		write_string_LCD("MAX VEL- ", 0x00, CLK);
		write_string_LCD(maxVel, 0x09, CLK);
		write_string_LCD("MAX ACCEL- ", 0x00, CLK);
		write_string_LCD(maxAccel, 0x0B, CLK);
	}
	else if (pageNum == 3)
	{
		write_string_LCD("0-60MPH TIME", 0x00, CLK);
		write_string_LCD(sixtyTime, 0x47, CLK);
	}
}

void measSixtyTime()
{
	//utilizes the timer 
	//begins timer if velocity is below 1 mph
	//stops  timer when velocity is greater than 60 mph
	//time is in ms so multiply by 1000
	
	//PROBALLY GOING TO OVERFLOW
	int time;
	
	if(velocity <= 0)
	{
		msCount = 0;
		time = 0;
	}
	else if (velocity >= 60)
	{
		time = msCount*1000
	}
	
	return time;
}

void TIMER_INIT()
{
    //TIMER_A0 setup
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = TIMER_DEL;               //Division needed for desired timer increments
    __enable_irq();                             //Enables global interrupts
}


void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
	TIMER_A0->CCR[0] += 120;
	
	msCount++;


}