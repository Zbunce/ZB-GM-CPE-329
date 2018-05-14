/*
 * UART.c
 * Encapsulates UART communication
 * getIntFlag, clrIntFlag, getRXByte, and sendByte_UART intended for external use
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

int getIntFlag_UART()
{
    return UART_FLG;
}

void clrIntFlag_UART()
{
    UART_FLG = 0;
}

char getRXByte()
{
    return RX_Val;
}

void UART_INIT()
{
    //Initializes UART comms
    EUSCI_A0 -> CTLW0 |=  EUSCI_A_CTLW0_SWRST;       //Reset
    EUSCI_A0 -> CTLW0  =  EUSCI_A_CTLW0_SWRST |      //Reset again
                          EUSCI_A_CTLW0_SSEL__SMCLK; //Sets UART to use SMCLK
    EUSCI_A0 -> BRW    =  1;                         //Baud rate related
    EUSCI_A0 -> MCTLW  =  (10 << EUSCI_A_MCTLW_BRF_OFS) |
                          EUSCI_A_MCTLW_OS16;   //Sets sampling modes
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  //Initializes FSM
    EUSCI_A0 -> IFG   &= ~EUSCI_A_IFG_RXIFG;    //Clears RX flag
    EUSCI_A0 -> IE    |=  EUSCI_A_IE_RXIE;      //Enables RX interrupts

    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    __enable_irq();                             //Enables global interrupts

    P1 -> SEL0 |= BIT2 | BIT3;                  //Initializes RX & TX
}

void sendByte_UART(uint8_t TX_Data)
{
    while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));  //Waits for TX flag to clear
    EUSCI_A0 -> TXBUF = TX_Data;    //Drops data into TX buffer
}

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
