/*
 * I2C.h
 * Encapsulates I2C management
 *
 * Date: May 22, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef I2C_H_
#define I2C_H_

#define ACCEL_ADDR 0x1D
#define EEPROM_ADDRESS 0x50

void I2C_INIT(uint8_t);
void writeByte_I2C(uint8_t, uint8_t);
uint8_t readByte_I2c(uint8_t);

void WriteEEPROM(uint16_t MemAddress, uint8_t MemByte);
uint8_t ReadEEPROM(uint16_t MemAddress);

#endif /* I2C_H_ */
