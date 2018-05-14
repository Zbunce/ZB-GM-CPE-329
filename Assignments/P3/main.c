/**
 * main.c
 */

#include "msp.h"
#include "ANSI.h"
#include "ADC.h"
#include "UART.h"
#include "set_DCO.h"
#include <string.h>

void DISP_INIT();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	int CLK = 30;
	set_DCO(CLK);
	UART_INIT();

	//while(1)
	DISP_INIT();

}

/* Spec:
 * 0-3v3 range
 * +-1 mV accuracy
 * Return average value over 1ms
 * Return DC offset of AC signals
 */
int measDCV_DMM()
{
    int DCV = 0;
    return DCV;
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
    int ACV = 0;
    return ACV;
}

//Subprocess of measACV
int measTRMS_DMM()
{
    int TRMS_Volt = 0;
    return TRMS_Volt;
}

//Subprocess of measACV
int measPktoPk_DMM()
{
    int PkPk_Volt = 0;
    return PkPk_Volt;
}

/* Spec:
 * 1-1000Hz range
 * +-1 Hz accuracy
 * Return frequency
 * Able to measure periodic waveforms (sin, tri, sqr, etc.)
 */
int measFreq_DMM()
{
    int freq = 0;
    return freq;
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
    int i;
    uint8_t col;
    uint8_t row;
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
    sendString_UART("|  DC/True-RMS  Voltage: 0.000V |");
    sendString_UART(" 3.3  3.3  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_5);
    sendString_UART("|  AC Peak-Peak Voltage: 0.000V |");
    sendString_UART(" 3.0  3.0  |");

    cursPhoneHome_ANSI();
    moveCursDown_ANSI(A_6);
    sendString_UART("|  DC Offset    Voltage: 0.000V |");
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
    sendString_UART("|  AC Frequency: 0000 Hz        |");
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
