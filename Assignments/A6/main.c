/**
 * main.c
 *
 * main code
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */
/*
#include "msp.h"
#include <stdint.h>
#include "DAC.h"
#include "SPI.h"
#include "set_DCO.h"
#include "delays.h"

void delay_ms(int, int);
void delay_us(int, int);
void set_DCO(int);
void makeDC(int);
void SPI_INIT();


int main (void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    set_DCO(CLK);

    SPI_INIT();

    while(1) {
        makeDC(2);
    }
}*/

//TIE LDAC LOW

//P1.5 SCLK
//P1.6 MOSI
//P1.7 MISO

#include "msp.h"
#include <stdint.h>
#include "set_DCO.h"
#include "delays.h"
#include "DAC.h"

void delay_ms(int, int);
void delay_us(int, int);
void set_DCO(int);

int main(void) {
    uint8_t data;
    uint16_t i;
    uint8_t up_Byte;
    uint8_t low_Byte;
    int CLK = 120;
    set_DCO(CLK);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    P5 -> DIR |= BIT5; //CS Output
    //P1 -> DIR |= (BIT5 | BIT6);
    P1 -> SEL0 |= (BIT5 | BIT6 | BIT7); //SPI Def
    P1 -> SEL1 &= ~(BIT5 | BIT6);
    P5 -> OUT |= BIT5;

    //Can use either style?
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST; //Reset
    EUSCI_B0 -> CTLW0 = EUSCI_B_CTLW0_SWRST |
                        EUSCI_B_CTLW0_MST |
                        EUSCI_B_CTLW0_SYNC |
                        EUSCI_B_CTLW0_CKPL |
                        EUSCI_B_CTLW0_MSB;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; //SMCLK

    EUSCI_B0 -> BRW = 0x01; //BRW = 1; SDICLK = SMCLK
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST; //Start FSM
    //Not really started, we're waiting to go as soon as we get TX
    EUSCI_B0 -> IFG |= EUSCI_B_IFG_TXIFG;
//    EUSCI_B0 -> IE |= EUSCI_B_IE_RXIE; //Enables ints or something
//    __enable_irq();
//    NVIC -> ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);//Sets ISR lookup
    //EUSCIB0 is NVIC port 20 = 10100, 31 = 11111, 20&31=20
    //Clears outside bits of NVIC port; ports above 31 need ISER[1]
    while(1) {
        //for (i = 0; i < 4096; i+= 500) {
        i = 0x30A5;
            P5->OUT &= ~BIT5;
            delay_us(100, CLK);
            //0x39B2
            //data = 0x39;

            up_Byte  =  ((i & 0x0F00) >> 8);
            up_Byte &= 0x0F;
            up_Byte |= (GAIN1 | SDOFF);
            low_Byte =   (i & 0x00FF);

            //Waits for TX flag to go low
            //while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));
            EUSCI_B0->TXBUF = up_Byte; //Drops data into buffer
            //for (i = 0; i < 20000; i++); //Waits a bit for no reason
            //data = 0x92;
            //Waits for TX flag to go low
            while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
            EUSCI_B0->TXBUF = low_Byte; //Drops data into buffer
            //for (i = 0; i < 20000; i++); //Waits a bit for no reason
            while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
            delay_us(100, CLK);
            P5->OUT |= BIT5;
            delay_ms(1, CLK);
        //}

    }
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
