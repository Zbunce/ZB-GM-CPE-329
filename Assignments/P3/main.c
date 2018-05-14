

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
#include <string.h>


static int msCount;
static int totalVoltVal;
volatile uint16_t captureValue[2] = {0,0};
volatile uint16_t captureFlag = 0;

void DISP_INIT();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	int CLK = 120;
	set_DCO(CLK);
	ADC_INIT();
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
	int totalVoltVal = 0;
	
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK 
				  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
	TIMER_A0->CCR[0] = 120; 	// math to get 10us -> 1/12Mhz = 83ns  10us/83ns = 120
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
	int ACV[2]= {0,0};
	ACV[0] = measPktoPk_DMM();
	ACV[1] = measTRMS_DMM();
	return ACV[];
}

//Subprocess of measACV
int measTRMS_DMM()
{
	// formula is VRMS = sqrt((v1^2+v2^2+....+Vn^2)/n)
	//to do this i need to sample a given amount of times based on the frequency
	//perferable the whole period then with those samples i can sqare them and then divide by # of samples
	// then just sqrt the whole thing   DO I NEED TO USE A DOUBLE?
	
	
	//FAKE RMS till i get the frequency working
	int PK2PK = measPktoPk_DMM;
	int TRMS = (PK2PK * 0.70710678118);
	return TRMS;
	
}

//Subprocess of measACV
int measPktoPk_DMM()
{
	int pk-pk = 0;
	int volt = calcVolt_ADC();
	int topVal = volt;
	int bottomVal = volt ;
	//NEED FREQUENCY TO TAKE SAMPLES OVER THE WHOLE PERIOD 
	for ( calcVolt_ADC() > topVal)
	{
		topVal = volt;
	}
	for ( calcVolt_ADC() < bottomVal)
	{
		bottomVal = volt;
	}
	pk-pk = topVal - bottomVal;
	return pk-pk;
}

/* Spec:
 * 1-1000Hz range
 * +-1 Hz accuracy
 * Return frequency
 * Able to measure periodic waveforms (sin, tri, sqr, etc.)
 */
int measFreq_DMM()
{
	//GERFENS CODE
	P2 -> SEL0 |= BIT5;
	P2 -> DIR  &= ~BIT5;
	
	TIMER_A0 -> CCTL[2] = 	TIMER_A_CCTL_CM_1 |
							TIMER_A_CCTL_CCIS_0 |
							TIMER_A_CCTL_CAP |
							TIMER_A_CCTL_SCS;
	TIMER_A0 -> CCTL =		TIMER_A_TASSEL_2 |
							TIMER_A_MC_CONTINUOUS |
							TIMER_A_CLR;
							
	NVIC -> ISER[0] = 1 << ((TA0_N_IRQn) &31);
	__ENABLE_IRQ();
	
	while(1)
	{
		if (captureFlag)
		{
			__DISABLE_IRQ();
			capturePeriod = captureValue[1] - captureValue[0];
			captureFlag = 0;
			__ENABLE_IRQ();
		}
	
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
    for(i = 0; i < 31; i++) 
	{
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
    for(i = 0; i < 31; i++) 
	{
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
    for(i = 0; i < 29; i++) 
	{
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

void TA0_0_IRQHandler(void) 
{
	//GERFENS CODE
	volatile static unint32_t captureCount -0;
	if (TIMER_A0 -> CCTL[2] = TIMER_A_CCTL_CCIFG)
	{
		captureValue [captureCount++] = TIMER_A0 -> CCR[2];
		if (captureCount ==2)
		{
			captureCount =0;
			captureFlag =1;
		}
		TIMER_A0 - CCTL[2] &= ~TIMER_A_CCTL_CCIFG;
	}
	
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
	TIMER_A0->CCR[0] += 120;
	msCount = msCount++;
	totalVoltVal = totalVoltVal + calcVolt_ADC();
}

