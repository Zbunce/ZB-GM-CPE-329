/*
 * UART.c
 * Encapsulates UART communication
 * getIntFlag, clrIntFlag, and getRXByte intended for external use
 *
 * Date: May 7, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "keypad.h"
#include <STDINT.h>

static int RX_Val = 0;
static int UART_FLG = 0;
//static uint16_t DN_Point = 0;

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
    //UART communications initialization
    EUSCI_A0 -> CTLW0 |=  EUSCI_A_CTLW0_SWRST;       //reset state machine
    EUSCI_A0 -> CTLW0  =  EUSCI_A_CTLW0_SWRST |      //reset state machine
                          EUSCI_A_CTLW0_SSEL__SMCLK; //use SMCLK
    EUSCI_A0 -> BRW    =  1;
    EUSCI_A0 -> MCTLW  =  (10 << EUSCI_A_MCTLW_BRF_OFS) |
                          EUSCI_A_MCTLW_OS16; //Bit offset//oversampling mode
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  //initialize state machine
    EUSCI_A0 -> IFG   &= ~EUSCI_A_IFG_RXIFG;    //clear receive interrupt flag
    EUSCI_A0 -> IE    |=  EUSCI_A_IE_RXIE;       //enable receive interrupt flag

    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    __enable_irq();                                 //enable global interrupts

    P1 -> SEL0 |= BIT2 | BIT3;                          //RX and TX respectively
}

void EUSCIA0_IRQHandler(void)
{
    if(EUSCI_A0 -> IFG & EUSCI_A_IFG_RXIFG) {
        while(!(EUSCI_A0 -> IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0 -> TXBUF = EUSCI_A0 -> RXBUF;
        RX_Val = EUSCI_A0 -> RXBUF;
        UART_FLG = 1;
    }
}
