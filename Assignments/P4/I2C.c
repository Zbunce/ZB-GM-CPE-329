/*
 * I2C.c
 * Encapsulates I2C management protocols based on Hummel's A9 example code
 *
 * Date: May 22, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include <stdint.h>
#include "I2C.h"

uint16_t TransmitFlag = 0;

//5 ms delay required btxt read/write

//Initializes I2C bus for communication with a device
//Requires SMCLK run at 12MHz
void I2C_INIT(uint8_t devAddr)
{

    P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |     // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |     // I2C mode
            EUSCI_B_CTLW0_MST |     // Master mode
            EUSCI_B_CTLW0_SYNC |     // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK; // SMCLK

//  EUSCI_B0->BRW    = 30;                        // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0->BRW = 30;                        // baudrate = SMCLK / 30 = 400kHz
    EUSCI_B0->I2CSA = devAddr;             // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    EUSCI_B0->IE |= EUSCI_A_IE_RXIE |             // Enable receive interrupt
            EUSCI_A_IE_TXIE;
}

//Writes single byte to specified device register
void writeByte_I2C(uint8_t regAddr, uint8_t data)
{
//    //Change slave address
//    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
//    EUSCI_B0->I2CSA  = ACCEL_ADDR;             // Slave address
//    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = regAddr;    // Sends the device register address
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = data;      // Sends the data byte
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

//Reads single byte from specified device register address
uint8_t readByte_I2C(uint8_t regAddr)
{
    //Change slave address
//    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
//    EUSCI_B0->I2CSA  = ACCEL_ADDR;             // Slave address
//    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    uint8_t ReceiveByte;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition
    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = regAddr;    // Sends the memory address
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)
    // Wait for start repeat to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag);            // Wait to receive a byte
    TransmitFlag = 0;

    ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

    return ReceiveByte;
}

//Reads specified number of sequential registers from accelerometer
//Have to use burst mode?
uint8_t readMulti_I2C(uint8_t regAddr, int seqReg)
{
    //Change slave address
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
    EUSCI_B0->I2CSA  = ACCEL_ADDR;             // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    uint8_t ReceiveByte;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition
    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = regAddr;    // Sends the memory address
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));



    // set stop bit to trigger
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag);            // Wait to receive a byte
    TransmitFlag = 0;

    ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

    return ReceiveByte;
}

//EEPROM used as nonvolatile memory to hold records
//Use 2kO pullup for 400kHz
//Have to reset accel addr after running

////////////////////////////////////////////////////////////////////////////////
//
//  Function that writes a single byte to the EEPROM.
//
//  MemAddress  - 2 byte address specifies the address in the EEPROM memory
//  MemByte     - 1 byte value that is stored in the EEPROM
//
//  Procedure :
//      start
//      transmit address+W (control+0)     -> ACK (from EEPROM)
//      transmit data      (high address)  -> ACK (from EEPROM)
//      transmit data      (low address)   -> ACK (from EEPROM)
//      transmit data      (data)          -> ACK (from EEPROM)
//      stop
//
////////////////////////////////////////////////////////////////////////////////
void WriteEEPROM(uint16_t MemAddress, uint8_t MemByte)
{

    //Change slave address
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
    EUSCI_B0->I2CSA  = EEPROM_ADDRESS;             // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset

    uint8_t HiAddress;
    uint8_t LoAddress;

    HiAddress = MemAddress >> 8;
    LoAddress = MemAddress & 0xFF;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = HiAddress;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->TXBUF = LoAddress;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = MemByte;      // Send the byte to store in EEPROM

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function that reads a single byte from the EEPROM.
//
//  MemAddress  - 2 byte address specifies the address in the EEPROM memory
//  ReceiveByte - 1 byte value that is received from the EEPROM
//
//  Procedure :
//      start
//      transmit address+W (control+0)    -> ACK (from EEPROM)
//      transmit data      (high address) -> ACK (from EEPROM)
//      transmit data      (low address)  -> ACK (from EEPROM)
//      start
//      transmit address+R (control+1)    -> ACK (from EEPROM)
//      transmit data      (data)         -> NACK (from MSP432)
//      stop
//
////////////////////////////////////////////////////////////////////////////////
uint8_t ReadEEPROM(uint16_t MemAddress)
{
    //Change slave address
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Software reset enabled
    EUSCI_B0->I2CSA  = EEPROM_ADDRESS;             // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;      // Release eUSCI from reset


    uint8_t ReceiveByte;
    uint8_t HiAddress;
    uint8_t LoAddress;

    HiAddress = MemAddress >> 8;
    LoAddress = MemAddress & 0xFF;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = HiAddress;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = LoAddress;    // Send the low byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag);            // Wait to receive a byte
    TransmitFlag = 0;

    ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

    return ReceiveByte;
}

////////////////////////////////////////////////////////////////////////////////
//
// I2C Interrupt Service Routine
//
////////////////////////////////////////////////////////////////////////////////
void EUSCIB0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }
}
