/**
 * SPI.c
 *
 * SPI Management
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

//P1.5 SCLK
//P1.6 MOSI
//P1.7 MISO

#include "msp.h"
#include <stdint.h>

void SPI_INIT()
{
    P1 -> SEL0 |= BIT5 + BIT6 + BIT7; //SPI Def
    //P2 -> DIR  |= BIT0 + BIT1 + BIT2; //LED Output

    EUSCI_B0 -> CTLW0 |=   EUSCI_B_CTLW0_SWRST;         //Reset
    EUSCI_B0 -> CTLW0  =   EUSCI_B_CTLW0_SWRST     |
                           EUSCI_B_CTLW0_MST       |
                           EUSCI_B_CTLW0_SYNC      |
                           EUSCI_B_CTLW0_CKPL      |
                           EUSCI_B_CTLW0_UCSSEL_2  |    //SMCLK
                           EUSCI_B_CTLW0_MSB;

    EUSCI_B0 -> BRW = 0x0001; //BRW = 1; SDICLK = SMCLK
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST; //Start FSM
    //Not really started, we're waiting to go as soon as we get TX
    EUSCI_B0 -> IE |= EUSCI_B_IE_RXIE; //Enables ints or something
    __enable_irq();
    NVIC -> ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);//Sets ISR lookup
    //EUSCIB0 is NVIC port 20 = 10100, 31 = 11111, 20&31=20
    //Clears outside bits of NVIC port; ports above 31 need ISER[1]
}

//void EUSCIB0_IRQHandler(void) {
//    volatile uint8_t RX_Data;
//    //Checks if RX flag went high
//    if (EUSCI_B0 -> IFG & EUSCI_B_IFG_RXIFG) {
//        RX_Data = EUSCI_B0 -> RXBUF;
//        P2 -> OUT &= ~(BIT0 | BIT1 | BIT2);
//        P2 -> OUT |= (RX_Data & 0x07);
//    }
//}

void sendByte_SPI(uint8_t data)
{
    uint32_t i;
    //Waits for TX flag to go low
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0 -> TXBUF = data; //Drops data into buffer
    for (i = 0; i < 20000; i++); //Waits a bit for no reason
}
