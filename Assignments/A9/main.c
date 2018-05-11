/*
 * main.c
 * Measures the analog voltage with the ADC
 * Prints voltage to the serial terminal connected to the UART
 *
 * Date: May 11, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "ADC.h"
#include "UART.h"
#include "set_DCO.h"

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int CLK = 30;
    int volt;
    int DR_ADC_FLG = 0;

	set_DCO(CLK);
	UART_INIT();
	ADC_INIT();

    while (1)
    {
        // Start sampling/conversion
        DR_ADC_FLG = getIntFlag_ADC();
        if (DR_ADC_FLG == 1) {
            volt = calcVolt_ADC();
            sendVolt_ADC(volt);
            clrIntFlag_ADC();
        }
    }
}




