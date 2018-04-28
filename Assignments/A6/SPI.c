/**
 * SPI.c
 *
 * Contains functions for both SPI setup and transmission
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

//P1.5 SCLK
//P1.6 MOSI
//P1.7 MISO

#include "msp.h"
#include <stdint.h>
#include "SPI.h"

void SPI_INIT()
{
    P5 -> DIR  |= BIT5;                         //CS Output
    P5 -> OUT  |= BIT5;                         //Initializes CS high
    P1 -> SEL0 |= (BIT5 | BIT6 | BIT7);         //SPI Line pinout setup
    P1 -> SEL1 &= ~(BIT5 | BIT6);

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;   //Reset
    EUSCI_B0 -> CTLW0  = EUSCI_B_CTLW0_SWRST |
                         EUSCI_B_CTLW0_MST   |
                         EUSCI_B_CTLW0_SYNC  |
                         EUSCI_B_CTLW0_CKPL  |
                         EUSCI_B_CTLW0_MSB;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; //SMCLK

    EUSCI_B0 -> BRW = 0x01;                     //BRW = 1; SDICLK = SMCLK
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  //Start FSM
    //SPI ready to go as soon as TX data dropped in buffer
    EUSCI_B0 -> IFG |= EUSCI_B_IFG_TXIFG;
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
    EUSCI_B0 -> TXBUF = data;                       //Drops data into buffer
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));   //Waits for TX flag to go low
}
