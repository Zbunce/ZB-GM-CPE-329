

/**
 * main.c
 */
 
 
#include "msp.h"
#include <stdint.h>
#include <math.h>
#include "set_DCO.h"
#include "delays.h"
#include "SPI.h"
#include "DAC.h"
#include "ADC.h"

static int msCount;


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	int CLK = 120;
	set_DCO(CLK);
	ADC_INIT();


}

/* Spec:
 * 0-3v3 range
 * +-1 mV accuracy
 * Return average value over 1ms
 * Return DC offset of AC signals
 */
int measDCV_DMM()
{
	int totalVoltVal = 0;
	
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK 
				  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
	TIMER_A0->CCR[0] = 10; 
	if ( msCount == 100)
	{
		averageVoltage = totalVoltVal/100;
		return averageVoltage; 
	}
}

/* Spec:
 * 0-3v3 range
 * +-1 mV accuracy
 * Return true RMS voltage
 * Return pk-pk voltage
 * Able to measure periodic waveforms (sin, tri, sqr, etc.)
 * Min measurable pk-pk: 0v5
 * Max measurable DC offset: 2v75
 */
int measACV_DMM()
{

}

//Subprocess of measACV
int measTRMS_DMM()
{

}

//Subprocess of measACV
int measPktoPk_DMM()
{

}

/* Spec:
 * 1-1000Hz range
 * +-1 Hz accuracy
 * Return frequency
 * Able to measure periodic waveforms (sin, tri, sqr, etc.)
 */
int measFreq_DMM()
{

}

/* Spec:
 * baud > 9600
 * Fields displayed in fixed locations
 * Display AC Voltage
 * Display DC Voltage
 * Display Frequency
 * Organize through borders
 * Bar graph of true RMS with scale
 * Bar graph for DC voltage with scale
 * Bar graphs will be a single line wide
 * Bar graphs will have set lengths?
 * Bar graphs will respond in real time
 */
//Updates values displayed in the terminal
void updateDisp_DMM()
{

}

//Initializes terminal display
void DISP_INIT()
{

}

void TA0_0_IRQHandler(void) 
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
	msCount = msCount++;
}

