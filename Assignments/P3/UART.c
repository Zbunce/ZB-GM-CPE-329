/*
 * UART.c
 * Encapsulates UART communication
 * getIntFlag, clrIntFlag, getRXByte, sendByte_UART, and sendString_UART intended for external use
 * UART_INIT must be run prior to the use of this library
 *
 * Date: May 11, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "UART.h"
#include <STDINT.h>
#include <strings.h>

static int RX_Val = 0;
static int UART_FLG = 0;

//Returns RX indicator flag
int getIntFlag_UART()
{
    return UART_FLG;
}

//Clears RX indicator flag
void clrIntFlag_UART()
{
    UART_FLG = 0;
}

//Returns RX data received
char getRXByte()
{
    return RX_Val;
}

//Initializes UART communications
void UART_INIT(int CLK, int baud)
{
    //Initializes UART comms
    EUSCI_A0 -> CTLW0 |=  EUSCI_A_CTLW0_SWRST;       //Reset
    EUSCI_A0 -> CTLW0  =  EUSCI_A_CTLW0_SWRST |      //Reset again
                          EUSCI_A_CTLW0_SSEL__SMCLK; //Sets UART to use SMCLK

    //Calculates register settings based on baud rate and clock speed used
    float N;
    float N1;
    float N1_Frac;
    float N1_FracShift;
    int tempInt;
    int N1FSInt;
    int BRW_Val;
    int BRS_Val;
    float baudDiv = baud / 100000.0000;
    N = CLK / baudDiv;
    N1 = N / 16;
    tempInt = N1;
    BRW_Val = tempInt;
    N1_Frac = N1 - tempInt;
    N1_FracShift = N1_Frac * 10000;
    tempInt = N1_FracShift;
    N1FSInt = tempInt;

    if (N1FSInt < 529) {
        BRS_Val = 0x00;
    }
    else if (N1FSInt < 715) {
        BRS_Val = 0x01;
    }
    else if (N1FSInt < 835) {
        BRS_Val = 0x02;
    }
    else if (N1FSInt < 1001) {
        BRS_Val = 0x04;
    }
    else if (N1FSInt < 1252) {
        BRS_Val = 0x08;
    }
    else if (N1FSInt < 1430) {
        BRS_Val = 0x10;
    }
    else if (N1FSInt < 1670) {
        BRS_Val = 0x20;
    }
    else if (N1FSInt < 2147) {
        BRS_Val = 0x11;
    }
    else if (N1FSInt < 2224) {
        BRS_Val = 0x21;
    }
    else if (N1FSInt < 2503) {
        BRS_Val = 0x22;
    }
    else if (N1FSInt < 3000) {
        BRS_Val = 0x44;
    }
    else if (N1FSInt < 3335) {
        BRS_Val = 0x25;
    }
    else if (N1FSInt < 3575) {
        BRS_Val = 0x49;
    }
    else if (N1FSInt < 3753) {
        BRS_Val = 0x4A;
    }
    else if (N1FSInt < 4003) {
        BRS_Val = 0x52;
    }
    else if (N1FSInt < 4286) {
        BRS_Val = 0x92;
    }
    else if (N1FSInt < 4378) {
        BRS_Val = 0x53;
    }
    else if (N1FSInt < 5002) {
        BRS_Val = 0x55;
    }
    else if (N1FSInt < 5715) {
        BRS_Val = 0xAA;
    }
    else if (N1FSInt < 6003) {
        BRS_Val = 0x6B;
    }
    else if (N1FSInt < 6254) {
        BRS_Val = 0xAD;
    }
    else if (N1FSInt < 6432) {
        BRS_Val = 0xB5;
    }
    else if (N1FSInt < 6667) {
        BRS_Val = 0xB6;
    }
    else if (N1FSInt < 7001) {
        BRS_Val = 0xD6;
    }
    else if (N1FSInt < 7147) {
        BRS_Val = 0xB7;
    }
    else if (N1FSInt < 7503) {
        BRS_Val = 0xBB;
    }
    else if (N1FSInt < 7861) {
        BRS_Val = 0xDD;
    }
    else if (N1FSInt < 8004) {
        BRS_Val = 0xED;
    }
    else if (N1FSInt < 8333) {
        BRS_Val = 0xEE;
    }
    else if (N1FSInt < 8464) {
        BRS_Val = 0xBF;
    }
    else if (N1FSInt < 8572) {
        BRS_Val = 0xDF;
    }
    else if (N1FSInt < 8751) {
        BRS_Val = 0xEF;
    }
    else if (N1FSInt < 9004) {
        BRS_Val = 0xF7;
    }
    else if (N1FSInt < 9170) {
        BRS_Val = 0xFB;
    }
    else if (N1FSInt < 9288) {
        BRS_Val = 0xFD;
    }
    else {
        BRS_Val = 0xFE;
    }

    EUSCI_A0 -> BRW    =  BRW_Val;              //Sets baud rate
    EUSCI_A0 -> MCTLW  =  (BRS_Val << EUSCI_A_MCTLW_BRF_OFS) |
                          EUSCI_A_MCTLW_OS16;   //Sets sampling modes

    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  //Initializes FSM
    EUSCI_A0 -> IFG   &= ~EUSCI_A_IFG_RXIFG;    //Clears RX flag
    EUSCI_A0 -> IE    |=  EUSCI_A_IE_RXIE;      //Enables RX interrupts

    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    __enable_irq();                             //Enables global interrupts

    P1 -> SEL0 |= BIT2 | BIT3;                  //Initializes RX & TX
}

//Sends the TX byte
void sendByte_UART(uint8_t TX_Data)
{
    while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));  //Waits for TX flag to clear
    EUSCI_A0 -> TXBUF = TX_Data;    //Drops data into TX buffer
}

//Sends a string of ASCII characters sequentially
void sendString_UART(uint8_t word[])
{
    uint8_t i;
    uint8_t len = strlen(word);
    for(i = 0; i < len; i++) {
        sendByte_UART(word[i]);
    }
}

//Handles RX Interrupt
void EUSCIA0_IRQHandler(void)
{
    if(EUSCI_A0 -> IFG & EUSCI_A_IFG_RXIFG) {
        while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));  //Waits for TX flag to clear
        EUSCI_A0 -> TXBUF = EUSCI_A0 -> RXBUF;  //Echo
        RX_Val = EUSCI_A0 -> RXBUF;             //Reads in RX character
        UART_FLG = 1;                           //Sets flag for external use
    }
}
