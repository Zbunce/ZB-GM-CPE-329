#include "msp.h"
#include <stdint.h>

int main (void)
{
    uint8_t data;
    uint32_t i;

    //stop watchdog

    P1 -> SEL0 |= BIT5 +BIT6 + BIT7; //SPI DEF
    P2 -> DIR |= BIT0 + BIT1 + BIT2; // LED output

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST; // RESET
    EUSCI_B0 -> CTLW0 = EUSCI_B_CTLW0_SWRST
                        | EUSCI_B_CTLW0_MST
                        | EUSCI_B_CTLW0_SYNC
                        | EUSCI_B_CTLW0_CKDL
                        | EUSCI_B_CTLW0_SSEC_2
                        | EUSCI_B_CTLW0_MSB;

    EUSCI_B0 -> BRW = 0X01; //BRW=1 SPICLK = SMCLK
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST; //START STATE MACHINE
    EUSCI_B0 -> IE |= EUSCI_B_IE_RXIE; //ENABLES

    __ENABLE_IRQ();
    NVIC -> ISER[0] = 1 << ((EUSCIB0_IRQn) & 31); //SETS ISR LOOKUP THING
    // EUSCIB0_IRQn IS NVIC PORT 20 = 10100(20) & 11111(31) = 10100

    WHILE (1)
    {
        FOR (data = 0;  data <8 ; data++)
        {
            WHILE(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG)); // WAITS FOR TRANSMIT
            EUSCI_B0 -> TX_BUF = data; // PUTS DATA IN BUFFER
            FOR(i = 0 ; i < 2000; i++); // WAITS FOR OBSERVATION
        }
    }


}

VOID EUSCIB0_IRQHandler(VOID)
{
    VOLITILE uint8_t RXData;
    IF( EUSCI_B0 -> IFG & EUSCI_B_IFG_RXIFG)
    {
        RXData = EUSCI_B0 -> RXBUF;
        P2 -> OUT &= ~(BIT0 | BIT1 | BIT2_;
        P2 -> OUT |= (RXData & 0X07);
    }
}
