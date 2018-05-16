/**
 * main.c
 */

#include "msp.h"
#include <stdint.h>
#include <math.h>
#include "set_DCO.h"
#include "delays.h"
#include "ADC.h"
#include "ANSI.h"
#include "UART.h"
#include <string.h>

//Run the program off globals, we'll encapsulate later. Way easier to manage than fxn passthrough
static int msCount;
//static int totalVoltVal;

//All measurements 4 digit HCD
static int ACVoltage = 0;   //Peak to Peak
static int DCVoltage = 0;   //DC/True RMS
static int frequency = 0;   //Frequency
static int DCOffset  = 0;   //DC Offset
static int AC_Flag = 0;     //High if AC detected

volatile uint16_t captureValue[2] = {0,0};
volatile uint16_t captureFlag = 0;

void DISP_INIT();
void measACV_DMM();
void measDCV_DMM();
void measTRMS_DMM();
void measPktoPk_DMM();
void measFreq_DMM();
void updateDisp_DMM();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 240;
    int z = 0;
    set_DCO(CLK);
    ADC_INIT();
    UART_INIT(CLK, 115200);
    DISP_INIT();

    while(1) {
//        AC_Flag = getACFlag_DMM();
        AC_Flag = 0;

        if (AC_Flag == 1) {
            measACV_DMM();
            measFreq_DMM();
        }
        else {
//            measDCV_DMM();
            ACVoltage = 0;
            frequency = 0;
            DCOffset = 0;
        }
        if (z == 10) {
            z = 0;
            DISP_INIT();
        }
        z++;
        updateDisp_DMM();

        if (DCVoltage == 0x3300) {
            DCVoltage = 0;
        }
        else if ((DCVoltage & 0x0FFF) == 0x0900) {
            DCVoltage += 0x1000;
            DCVoltage -= 0x0900;
        }
        else {
            DCVoltage += 0x0300;
        }
        delay_ms(500, CLK);
    }
}

void TIMERS_INIT()
{

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = 120;     // math to get 10us -> 1/12Mhz = 83ns  10us/83ns = 120
}

/* Spec:
 * 0-3v3 range
 * +-1 mV accuracy
 * Return average value over 1ms
 * Return DC offset of AC signals
 */
void measDCV_DMM()
{
//    int totalVoltVal = 0;

    if ( msCount == 100)
    {
//        averageVoltage = totalVoltVal/100;
//        return averageVoltage;
    }
//    return 0;
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
void measACV_DMM()
{
    measTRMS_DMM();
    measPktoPk_DMM();
}

//Subprocess of measACV
void measTRMS_DMM()
{
    // formula is VRMS = sqrt((v1^2+v2^2+....+Vn^2)/n)
    //to do this i need to sample a given amount of times based on the frequency
    //perferable the whole period then with those samples i can sqare them and then divide by # of samples
    // then just sqrt the whole thing   DO I NEED TO USE A DOUBLE?


    //FAKE RMS till i get the frequency working
//    int PK2PK = measPktoPk_DMM();
//    int TRMS = (PK2PK * 0.70710678118);
//    return TRMS;

}

//Subprocess of measACV
void measPktoPk_DMM()
{
    int pkpk = 0;
    int volt = calcVolt_ADC();
//    int topVal = volt;
//    int bottomVal = volt;
    //NEED FREQUENCY TO TAKE SAMPLES OVER THE WHOLE PERIOD
//    for ( calcVolt_ADC() > topVal)
//    {
//        topVal = volt;
//    }
//    for ( calcVolt_ADC() < bottomVal)
//    {
//        bottomVal = volt;
//    }
//    pk-pk = topVal - bottomVal;
//    return pkpk;
}

/* Spec:
 * 1-1000Hz range
 * +-1 Hz accuracy
 * Return frequency
 * Able to measure periodic waveforms (sin, tri, sqr, etc.)
 */
void measFreq_DMM()
{
    //GERFENS CODE
    P2 -> SEL0 |= BIT5;
    P2 -> DIR  &= ~BIT5;

//    TIMER_A0 -> CCTL[2] =   TIMER_A_CCTL_CM_1 |
//                            TIMER_A_CCTL_CCIS_0 |
//                            TIMER_A_CCTL_CAP |
//                            TIMER_A_CCTL_SCS;
//    TIMER_A0 -> CCTL =      TIMER_A_TA0SSEL_2 |
//                            TIMER_A_MC_CONTINUOUS |
//                            TIMER_A_CLR;

    NVIC -> ISER[0] = 1 << ((TA0_N_IRQn) &31);
    __enable_irq();

    while(1)
    {
        if (captureFlag)
        {
            __disable_irq();
//            capturePeriod = captureValue[1] - captureValue[0];
            captureFlag = 0;
            __enable_irq();
        }

    }
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
    int meter;
    int antimeter;
    int i;

    uint8_t ones;
    uint8_t tenths;
    uint8_t hunths;
    uint8_t towths;

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
    antimeter = 12 - meter;

    cursPhoneHome_ANSI();

    if (AC_Flag == 1) {
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_9);
        moveCursDown_ANSI(A_8);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_6);
        for(i = 0; i <= meter; i++) {
            sendByte_UART(A_BLOCK);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }
        for(i = 0; i <= antimeter; i++) {
            sendByte_UART(A_SPACE);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }

        //True RMS
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_5);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_8);
        ones =   ((DCVoltage & 0xF000) >> 12) | 0x30;
        tenths = ((DCVoltage & 0x0F00) >> 8)  | 0x30;
        hunths = ((DCVoltage & 0x00F0) >> 4)  | 0x30;
        towths = (DCVoltage & 0x000F) | 0x30;
        sendByte_UART(ones);
        sendByte_UART(A_DECML);
        sendByte_UART(tenths);
        sendByte_UART(hunths);
        sendByte_UART(towths);
        sendByte_UART(A_V);

        //Peak to Peak
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_6);
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

        //DC Offset
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_7);
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

        //Frequency
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        ones =   ((frequency >> 12)& 0x0F) | 0x30;  //Totally not ones place but go with it
        tenths = ((frequency >> 8) & 0x0F) | 0x30;
        hunths = ((frequency >> 4) & 0x0F) | 0x30;
        towths = (frequency & 0x0F) | 0x30;
        sendByte_UART(ones);
        sendByte_UART(tenths);
        sendByte_UART(hunths);
        sendByte_UART(towths);
    }
    else {
        cursPhoneHome_ANSI();
        moveCursDown_ANSI(A_9);
        moveCursDown_ANSI(A_6);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_9);
        moveCursRight_ANSI(A_1);
        for(i = 0; i < meter; i++) {
            sendByte_UART(A_BLOCK);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }
        for(i = 0; i <= antimeter; i++) {
            sendByte_UART(A_SPACE);
            moveCursLeft_ANSI(A_1);
            moveCursUp_ANSI(A_1);
        }

        //DC Voltage
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
}


//Initializes terminal display
void DISP_INIT()
{
    int i;
//    uint8_t col;
//    uint8_t row;
    clearScreen_ANSI();
    cursPhoneHome_ANSI();

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

    cursPhoneHome_ANSI();
}
