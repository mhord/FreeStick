#include <project.h>
#include "sx1509.h"
#include "i2c_support.h"

uint8_t readByte(uint8_t slaveAddr, uint8_t regID)
{
  uint8_t retVal;
  I2C_MasterSendStart(slaveAddr, 0);
  I2C_MasterWriteByte(regID);
  I2C_MasterSendRestart(slaveAddr, 1);
  retVal = I2C_MasterReadByte(I2C_NAK_DATA);
  I2C_MasterSendStop();
  return retVal;
}

void writeByte(uint8_t slaveAddr, uint8_t regID, uint8_t data)
{
  I2C_MasterSendStart(slaveAddr, 0);
  I2C_MasterWriteByte(regID);
  I2C_MasterWriteByte(data);
  I2C_MasterSendStop();
}

uint16_t readWord(uint8_t slaveAddr, uint8_t regID)
{
  uint16_t retVal;
  I2C_MasterSendStart(slaveAddr, 0);
  I2C_MasterWriteByte(regID);
  I2C_MasterSendRestart(slaveAddr, 1);
  *(&retVal) = I2C_MasterReadByte(I2C_ACK_DATA);
  *(&retVal+1) = I2C_MasterReadByte(I2C_NAK_DATA);
  I2C_MasterSendStop();
  return retVal;
}

void writeWord(uint8_t slaveAddr, uint8_t regID, uint16_t data)
{
  I2C_MasterSendStart(slaveAddr, 0);
  I2C_MasterWriteByte(regID);
  I2C_MasterWriteByte(*(&data));
  I2C_MasterWriteByte(*(&data+1));
  I2C_MasterSendStop();
}

