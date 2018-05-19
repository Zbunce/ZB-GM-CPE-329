/**
 * main.c
 *
 * Runs the digital multimeter and handles the terminal update protocol
 */

#include "msp.h"
#include <stdint.h>
#include <math.h>
#include "set_DCO.h"
#include "delays.h"
#include "ADC.h"
#include "ANSI.h"
#include "UART.h"
#include "DMM.h"
#include <string.h>

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 240;
    int z = 0;

    int ACFlag = 0;
    int DCV;

    //Initializes various modules
    __disable_irq();
    set_DCO(CLK);
    ADC_INIT();
    TIMER_INIT();
    UART_INIT(CLK, 115200);
    DISP_INIT();
    __enable_irq();

    while(1) {
        ACFlag = getACFlag_DMM();   //Checks if AC/DC

        measPktoPk_DMM();           //Measures Peak to Peak voltage
        measDCV_DMM();              //Measures DC voltage

        DCV = getDCV_DMM();         //Grabs DC voltage

        if (ACFlag == 1) {
            setDCOff_DMM(DCV);      //Sets offset to averaged voltage

            measTRMS_DMM();         //Measures the TRMS internally
            measFreq_DMM();         //Measures the frequency internally
        }
        else {
            setDCV_DMM(DCV);        //Sets the DC voltage
            setFreq_DMM(0);         //Sets the frequency to 0
            setDCOff_DMM(0);        //Sets the DC offset to 0
        }

        if (z == 10) {
            z = 0;
            DISP_INIT();    //Soft resets the terminal formatting
        }

        z++;
        updateDisp_DMM();   //Updates the measurements displayed
        delay_ms(1, CLK);   //Takes a break and samples a bit
    }
}


