/*
 * I2C.c
 * Encapsulates I2C management
 *
 * Date: May 22, 2018
 * Author: Zach Bunce, Garrett Maxon
 */
/*
#include "msp.h"
#include <stdint.h>

#define TX_DATA_SIZE    5

uint8_t RXData[5] = {0};
uint8_t RXDataPointer;

// Pointer to TX data
uint8_t TXData[]= {0xA1, 0xB1, 0xC1, 0xD1};

// Define 4 slave addresses
uint8_t SlaveAddress[]= {0x0A, 0x0B, 0x0C, 0x0D};

uint8_t TXByteCtr;
uint8_t SlaveFlag = 0;

void I2C_INIT()
{
    // Configure GPIO
    P1->OUT  &= ~BIT0;                      // Clear P1.0 output latch
    P1->DIR  |= BIT0;                       // For LED
    P1->SEL0 |= BIT6 | BIT7;                // I2C pins

    // Initialize RX data variable
    RXDataPointer = 0;

    // Initialize TX data variable
//    TXData = 0;

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC -> ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
    EUSCI_B0 -> CTLW0  = EUSCI_A_CTLW0_SWRST  |     // Remain eUSCI in reset mode
                         EUSCI_B_CTLW0_MODE_3 |     // I2C mode
                         EUSCI_B_CTLW0_MST    |     // Master mode
                         EUSCI_B_CTLW0_SYNC   |     // Sync mode
                         EUSCI_B_CTLW0_SSEL__SMCLK; // SMCLK
    EUSCI_B0 -> CTLW1 |= EUSCI_B_CTLW1_ASTP_2;      // Automatic stop generated
                                                    // after EUSCI_B0->TBCNT is reached
    EUSCI_B0 -> BRW    = 30;                        // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0 -> TBCNT  = 0x0005;                    // number of bytes to be received
    EUSCI_B0 -> I2CSA  = 0x0048;                    // Slave address
    EUSCI_B0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    EUSCI_B0 -> IE |= EUSCI_A_IE_RXIE   |           // Enable receive interrupt
                      EUSCI_B_IE_NACKIE |           // Enable NACK interrupt
                      EUSCI_B_IE_BCNTIE;            // Enable byte counter interrupt
}


int main(void)
{
    volatile uint32_t i;
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    // Configure Pins for I2C
    P1->SEL0 |= BIT6 | BIT7;                // I2C pins

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // put eUSCI_B in reset state
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI_B in reset state
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // I2C master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK
    EUSCI_B0->BRW = 0x001E;                 // baudrate = SMCLK /30
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// clear reset register
    EUSCI_B0->IE |= EUSCI_B_IE_TXIE0 |      // Enable transmit interrupt
            EUSCI_B_IE_NACKIE;              // Enable NACK interrupt

    // Initialize slave flag
    SlaveFlag = 0;

    while(1)
    {
        // Delay between transmissions
        for (i = 1000; i > 0; i--);

        // configure slave address
        EUSCI_B0->I2CSA = SlaveAddress[SlaveFlag];

        // Load TX byte counter
        TXByteCtr = 1;

        // Ensure stop condition got sent
        while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);

        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR | // I2C TX
                EUSCI_B_CTLW0_TXSTT;        // Start condition

        // Change Slave address
        SlaveFlag++;

        // Roll over slave address
        if (SlaveFlag > 3)
        {
            SlaveFlag = 0;
        }
    }
}

// I2C interrupt service routine
void EUSCIB0_IRQHandler(void)
{
}

void sendByteTX(uint8_t RX)
{

}

// I2C interrupt service routine
void EUSCIB0_IRQHandler(void)
{
    //RX ISR
    if (EUSCI_B0->IFG & EUSCI_B_IFG_NACKIFG) {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_NACKIFG;

        // I2C start condition
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0) {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;

        // Get RX data
        RXData[RXDataPointer++] = EUSCI_B0->RXBUF;

        if (RXDataPointer > sizeof(RXData)) {
            RXDataPointer = 0;
        }
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_BCNTIFG) {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_BCNTIFG;
    }

    //TX ISR
    if (EUSCI_B0->IFG & EUSCI_B_IFG_NACKIFG) {
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_NACKIFG;

        // I2C start condition
        UCB0CTL1 |= EUSCI_B_CTLW0_TXSTT;
    }
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0) {
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;

        // Check TX byte counter
        if (TXByteCtr) {
            // Load TX buffer
            EUSCI_B0->TXBUF = TXData[SlaveFlag];

            // Decrement TX byte counter
            TXByteCtr--;
        }
        else {
            // I2C stop condition
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

            // Clear USCI_B0 TX int flag
            EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG;
        }
    }
}
*/
