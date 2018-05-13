#include "msp.h"


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	int CLK = 30;
	set_DCO(CLK);


}

/* Spec:
 * 0-3v3 range
 * +-1 mV accuracy
 * Return average value over 1ms
 * Return DC offset of AC signals
 */
int measDCV_DMM()
{

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
