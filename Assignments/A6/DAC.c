#include "msp.h"
#include <stdint.h>
#include "SPI.h"

void sendByte_SPI(uint8_t);
void SPI_INIT(void);

void write_DAC (uint16_t data)
{

    uint8_t up_Byte =  ((data & 0xFF00) >> 0x08);
    uint8_t low_Byte =  (data & 0x00FF);

    P2 -> DIR |= BIT4;
    P2 -> OUT &= ~BIT4;

    sendByte_SPI(up_Byte);
    sendByte_SPI(low_Byte);

    P2 -> OUT |= BIT4;

}

