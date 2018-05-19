/*
 * DMM.c
 *
 * Contains functions for managing the digital multimeter measurements
 * Also contains terminal management protocols
 * setDCV, setPktoPk, setTRMS, setFreq, and getACFlag intended for external use
 * TIMER_INIT and DISP_INIT must be run prior to the use of this library
 *
 * Date: May 18, 2018
 * Author: Zach Bunce, Garrett Maxon
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

//Local globals used to avoid function passthrough
static int msCount;             //Sample index counter
static int periodCount = 0;     //Edge counter
static int msCount2;            //Frequency time increment counter
static int schST_old = 0;       //Schmitt trigger state indicators
static int schST_new = 0;
static int edge1Time;           //Reference time for edge start
static int delT;                //Time difference value between edges
static int freqIdx = 0;         //Frequency measurement index counter

//All measurements 4 digit HCD
static int ACVoltage = 0;       //Peak to Peak voltage
static int DCVoltage = 0;       //DC/True RMS voltage
static int frequency = 0;       //Frequency
static int DCOffset  = 0;       //DC Offset voltage
static int AC_Flag = 0;         //High if AC detected
const int sampNum = 10000;      //Number of samples averaged over
static int vSample[sampNum];    //ADC sample array
static int freqArr[10];         //Frequency sample array

//Externally sets DC voltage sent to display
void setDCV_DMM(int DC)
{
    DCVoltage = DC;
}

//Externally sets AC Peak to Peak voltage sent to display
void setPktoPk_DMM(int ACpkpk)
{
    ACVoltage = ACpkpk;
}

//Externally sets AC True RMS voltage sent to display
void setTRMS_DMM(int TRMS)
{
    DCVoltage = TRMS;
}

//Externally sets DC offset voltage sent to display
void setDCOff_DMM(int DCOff)
{
    DCOffset = DCOff;
}

//Externally sets AC frequency sent to display
void setFreq_DMM(int freq)
{
    frequency = freq;
}

//Returns DC voltage measured
int getDCV_DMM()
{
    return DCVoltage;
}

//Returns AC indicator flag
int getACFlag_DMM()
{
    return AC_Flag;
}

//Initializes TIMER_A0 for counting 10us increments and the Schmitt trigger input pin
void TIMER_INIT()
{
    //Configures P2.4 to act as the frequency state monitor
    P2 -> DIR &= BIT4;
    P2 -> REN |= BIT4;
    P2 -> OUT &= ~BIT4;

    //TIMER_A0 setup
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = TIMER_DEL;               //Division needed for desired timer increments
    __enable_irq();                             //Enables global interrupts
}

//Calculates the average voltage over the ADC sample range stored
//Used for both DC voltage and DC offset measurements
void measDCV_DMM()
{
    int averageSample = 0;
    int sampleTotal = 0;
    int i;

    //Calculates the sum of all ADC samples
    for(i = 0; i < sampNum; i++)
    {
        sampleTotal += vSample[i];
    }
    averageSample = sampleTotal/sampNum;        //Averages the ADC sample sum
    DCVoltage = calcVolt_ADC(averageSample);    //Calculates the corresponding DC voltage
}

//Calculates the True-RMS voltage from the available ADC samples
void measTRMS_DMM()
{
    double squareTotal;
    int ADCNum;
    int i;

    //Calculates the sum of the squared ADC samples
    for(i = 0; i < sampNum; i++)
    {
        squareTotal += (vSample[i] * vSample[i]);
    }

    ADCNum = sqrt(squareTotal / sampNum);   //Calculates the root of the average of squares
    DCVoltage = calcVolt_ADC(ADCNum);       //Calculates the corresponding True RMS voltage
}

//Measures the peak to peak voltage difference in the ADC samples stored
void measPktoPk_DMM()
{
    int i;
    int topVal = (vSample[0])+1; //Initialized with different values so the loop has no issues
    int bottomVal = (vSample[0])-1;
    int pkpk;

    //Checks each sample to see if it is the max or min value
    for(i = 0; i < sampNum; i++)
    {
        if(vSample[i] > topVal)
        {
            topVal = vSample[i];
        }
        if(vSample[i] < bottomVal)
        {
            bottomVal = vSample[i];
        }
    }

    pkpk = topVal - bottomVal;  //Calculates the difference between the max and min

    //Checks if the peak to peak is below the AC threshold of 0.5V required
    if (pkpk > 2482 ) {
        AC_Flag = 1;
    }
    else {
        AC_Flag = 0;
    }

    ACVoltage = calcVolt_ADC(pkpk); //Calculates corresponding AC voltage
}

//Averages the last ten measured frequencies
void measFreq_DMM()
{
    int sampleTotal = 0;
    int i;

    //Resets frequency index
    if (freqIdx >= 10) {
        freqIdx = 0;
    }

    //Resets half period counter
    if (periodCount > 3) {
        periodCount = 0;
    }

    //Calculates the sum of frequencies sampled
    for(i = 0; i < 10; i++)
    {
        sampleTotal += freqArr[i];
    }

    frequency = sampleTotal/20; //Averages frequencies measured
}

//Updates measured values and graphs displayed in the terminal
void updateDisp_DMM()
{
    int meter;
    int antimeter;
    int i;

    uint8_t ones;
    uint8_t tenths;
    uint8_t hunths;
    uint8_t towths;

    __disable_irq();
    hideCurs_ANSI();    //Disables the blinking cursor

    //Calculates bar graph length
    if (DCVoltage < 0x0300) {
        meter = 1;
    }
    else if(DCVoltage < 0x0600) {
        meter = 2;
    }
    else if(DCVoltage < 0x0900) {
        meter = 3;
    }
    else if(DCVoltage < 0x1200) {
        meter = 4;
    }
    else if(DCVoltage < 0x1500) {
        meter = 5;
    }
    else if(DCVoltage < 0x1800) {
        meter = 6;
    }
    else if(DCVoltage < 0x2100) {
        meter = 7;
    }
    else if(DCVoltage < 0x2400) {
        meter = 8;
    }
    else if(DCVoltage < 0x2700) {
        meter = 9;
    }
    else if(DCVoltage < 0x3000) {
        meter = 10;
    }
    else if(DCVoltage < 0x3300) {
        meter = 11;
    }
    else {
        meter = 12;
    }
    antimeter = 12 - meter; //Calculates left over blocks which need clearing

    cursPhoneHome_ANSI();   //Returns cursor to known location

    //Updates the AC measurement displays
    if (AC_Flag == 1) {
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_9);
        moveCursDown_ANSI(A_6);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_6);

        //Plots the ACV bar graph
        for(i = 0; i <= meter; i++) {
            sendByte_UART(A_BLOCK);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }
        //Clears the extra spaces
        for(i = 0; i <= antimeter; i++) {
            sendByte_UART(A_SPACE);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }

        //True RMS update
        cursPhoneHome_ANSI();       //Returns cursor to known location
        moveCursDown_ANSI(A_4);     //Moves cursor down
        moveCursRight_ANSI(A_9);    //Moves cursor right
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_8);
        ones =   ((DCVoltage & 0xF000) >> 12) | 0x30;   //ASCII encodes ones place
        tenths = ((DCVoltage & 0x0F00) >> 8)  | 0x30;   //ASCII encodes tenths place
        hunths = ((DCVoltage & 0x00F0) >> 4)  | 0x30;   //ASCII encodes hundredths place
        towths = (DCVoltage & 0x000F) | 0x30;           //ASCII encodes thousandths place
        sendByte_UART(ones);                            //Sends ones place
        sendByte_UART(A_DECML);                         //Sends decimal point
        sendByte_UART(tenths);                          //Sends tenths place
        sendByte_UART(hunths);                          //Sends hundredths place
        sendByte_UART(towths);                          //Sends thousandths place
        sendByte_UART(A_V);                             //Sends units

        //Peak to Peak update
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_5);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_8);
        ones =   ((ACVoltage >> 12) & 0x0F) | 0x30;
        tenths = ((ACVoltage >> 8)  & 0x0F) | 0x30;
        hunths = ((ACVoltage >> 4)  & 0x0F) | 0x30;
        towths = (ACVoltage & 0x0F) | 0x30;
        sendByte_UART(ones);
        sendByte_UART(A_DECML);
        sendByte_UART(tenths);
        sendByte_UART(hunths);
        sendByte_UART(towths);
        sendByte_UART(A_V);

        //DC Offset update
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_6);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_8);
        ones =   ((DCOffset >> 12) & 0x0F) | 0x30;
        tenths = ((DCOffset >> 8)  & 0x0F) | 0x30;
        hunths = ((DCOffset >> 4)  & 0x0F) | 0x30;
        towths = (DCOffset & 0x0F) | 0x30;
        sendByte_UART(ones);
        sendByte_UART(A_DECML);
        sendByte_UART(tenths);
        sendByte_UART(hunths);
        sendByte_UART(towths);
        sendByte_UART(A_V);

        //Frequency update
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_8);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        towths = (frequency / 1000);
        hunths = (frequency / 100 - (towths*10));
        tenths = (frequency / 10 - ((towths*100) + (hunths*10)));
        ones   = (frequency - ((towths*1000) + (hunths*100) + (tenths*10)));  //Totally not ones place but go with it

        sendByte_UART(towths | 0x30);
        sendByte_UART(hunths | 0x30);
        sendByte_UART(tenths | 0x30);
        sendByte_UART(ones | 0x30);
    }
    else {
        //DC Voltage measurement values updated
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_9);
        moveCursDown_ANSI(A_6);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_1);

        //Updates DCV bar graph
        for(i = 0; i < meter; i++) {
            sendByte_UART(A_BLOCK);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }
        //Clears empty DCV spaces
        for(i = 0; i <= antimeter; i++) {
            sendByte_UART(A_SPACE);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }

        //DC Voltage update
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_4);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_7);
        ones =   ((DCVoltage >> 12) & 0x0F) | 0x30;
        tenths = ((DCVoltage >> 8)  & 0x0F) | 0x30;
        hunths = ((DCVoltage >> 4)  & 0x0F) | 0x30;
        towths = (DCVoltage & 0x0F) | 0x30;
        sendByte_UART(ones);
        sendByte_UART(A_DECML);
        sendByte_UART(tenths);
        sendByte_UART(hunths);
        sendByte_UART(towths);
        sendByte_UART(A_V);

        //Clear Peak to Peak
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_5);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_7);
        sendString_UART("N/A   ");

        //Clear DC Offset
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_6);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_7);
        sendString_UART("N/A   ");

        //Clear Frequency
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_8);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_8);
        sendString_UART("   0");
    }
    __enable_irq();
}

//Initializes terminal display
void DISP_INIT()
{
    int i;
    __disable_irq();
    clearScreen_ANSI();     //Clears the terminal
    hideCurs_ANSI();        //Turns off cursor blink
    cursPhoneHome_ANSI();   //Returns cursor to known location

    //Creates various terminal formatting, with steps broken up by row
    moveCursRight_ANSI(A_2);
    for(i = 0; i < 29; i++) {
        sendByte_UART(A_UNDSC);
    }
    sendString_UART("   _________");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_1);
    sendString_UART("_/      DIGITAL MULTIMETER     ");
    sendByte_UART(A_FSLSH);
    sendByte_UART(A_UNDSC);
    sendString_UART("/         ");
    sendByte_UART(A_FSLSH);
    sendByte_UART(A_UNDSC);

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_2);
    sendString_UART("|    ZACH BUNCE GARRETT MAXON   |");
    sendString_UART(" DCV  ACV  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_3);
    sendByte_UART(A_SSLSH);
    for(i = 0; i < 31; i++) {
        sendByte_UART(A_SPACE);
    }
    sendString_UART("|  V   VRMS |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_4);
    sendString_UART("|  DC/True-RMS  Voltage: ");
    moveCursRight_ANSI(A_6);
    sendString_UART(" |");
    sendString_UART(" 3.3  3.3  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_5);
    sendString_UART("|  AC Peak-Peak Voltage: ");
    moveCursRight_ANSI(A_6);
    sendString_UART(" |");
    sendString_UART(" 3.0  3.0  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_6);
    sendString_UART("|  DC Offset    Voltage: ");
    moveCursRight_ANSI(A_6);
    sendString_UART(" |");
    sendString_UART(" 2.7  2.7  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_7);
    sendByte_UART(A_SSLSH);
    for(i = 0; i < 31; i++) {
        sendByte_UART(A_SPACE);
    }
    sendString_UART("| 2.4  2.4  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_8);
    sendString_UART("|  AC Frequency: ");
    moveCursRight_ANSI(A_4);
    sendString_UART(" Hz        |");
    sendString_UART(" 2.1  2.1  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_9);
    moveCursRight_ANSI(A_1);
    sendByte_UART(A_FSLSH);
    for(i = 0; i < 29; i++) {
        sendByte_UART(A_UNDSC);
    }
    sendString_UART("/| 1.8  1.8  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 1.5  1.5  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 1.2  1.2  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 0.9  0.9  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 0.6  0.6  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 0.3  0.3  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_4);
    moveCursDown_ANSI(A_1);
    sendString_UART("| 0.0  0.0  |");

    moveCursLeft_ANSI(A_9);
    moveCursLeft_ANSI(A_3);
    moveCursDown_ANSI(A_1);
    sendByte_UART(A_FSLSH);
    sendString_UART("_________/");

    cursPhoneHome_ANSI();   //Returns cursor to known location

    __enable_irq();
}

//ISR for the TIMER_A0 10us increment counter
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
    TIMER_A0->CCR[0] += TIMER_DEL;              //Increments timer count for next 10us

    vSample[msCount] = getAnData_ADC();         //Pulls current ADC value, regardless of whether its ready
    clrIntFlag_ADC();                           //Enables next ADC sample

    schST_new = P2 -> IN & BIT4;                //Checks if the Schmitt trigger wave has switched states
    if (schST_new != schST_old) {               //Increments edge counter if so
        periodCount++;
    }

    msCount2++;                                 //Increments frequency counter

    //Handles the edge based time measurements and calculates frequency
    switch(periodCount) {
    case(1):
        edge1Time = msCount2;
        break;
    case(3):
        periodCount = 0;
        delT = msCount2 - edge1Time;
        msCount2 = 0;
        freqArr[freqIdx] = FREQ_MULT / delT;
        freqIdx++;
        break;
    }

    schST_old = schST_new;  //Cycles trigger state indicator

    //Resets sample array index counter
    switch(msCount) {
    case(sampNum - 1):
        msCount = 0;
        break;
    default:
        msCount++;
        break;
    }
}
