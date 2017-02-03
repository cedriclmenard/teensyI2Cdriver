#ifndef I2Cdev_HPP
#define I2Cdev_HPP

#include <stdint.h>
#include "mpu6050.h"
#include "kinetis.h"

#define NUM_I2C_CALLBACKS 16
#define I2C_RW_BUFFER_SIZE 64
static uint8_t currentNumberOfI2CCallbacks = 0;

extern void dummy_read_I2C_data(void);

class BlockingI2Cdev {
private:
  uint8_t _address;
  void waitAck();
public:
  BlockingI2Cdev(uint8_t address);
  void writeRegister(uint8_t reg,uint8_t data);
  uint8_t readRegister(uint8_t reg);
  void write(uint8_t data);
  void writeRegisterBurst(uint8_t reg, uint8_t data[], uint32_t length);
  void readRegisterBurst(uint8_t reg, uint8_t *data, uint32_t length);



};

enum I2CState : uint8_t  {
  TRANSMIT,
  RECEIVE,
  IDLE
};
enum I2CWriteMode : uint8_t {
  ADDRESSING,
  DATA
};

class I2Cdev {
private:
  uint8_t _address;
  uint8_t _callbackNumber;
  I2CState _currentState = IDLE;
  I2CWriteMode _currentWriteMode = ADDRESSING;
  uint32_t _currentWriteIndex = 0;
  uint32_t _currentReadIndex = 0;
  uint32_t _bytesToWrite = 0;
  uint32_t _bytesToRead = 0;
  //uint8_t _rwBuffer[I2C_RW_BUFFER_SIZE];
  uint8_t* _rwBuffer;


public:
  I2Cdev(uint8_t address);
  void write(uint8_t data[], uint32_t length);
  void read(uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *data, uint32_t length));
  void callback();




  // Initialize I2C peripheral hardware
  static void initializeI2C0(uint32_t frequency);

};

void (*i2c0_callback[NUM_I2C_CALLBACKS])(void) = NULL;

//void i2c0_isr();



#endif
