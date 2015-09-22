#ifndef __i2c_support_h__
#define __i2c_support_h__

#define KEYPAD_ADDR 0x3E
#define LED_ADDR    0x3F

uint8_t readByte(uint8_t slaveAddr, uint8_t regID);
void writeByte(uint8_t slaveAddr, uint8_t regID, uint8_t data);
uint16_t readWord(uint8_t slaveAddr, uint8_t regID);
void writeWord(uint8_t slaveAddr, uint8_t regID, uint16_t data);

#endif

