#ifndef I2Cdev_HPP
#define I2Cdev_HPP

#include <stdint.h>
#include "kinetis.h"
#include "Arduino.h"

#define NUM_I2C_CALLBACKS 16
#define I2C_RW_BUFFER_SIZE 64

static uint8_t currentNumberOfI2CCallbacks = 0;



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
  void setAddress(uint8_t address);
  uint8_t getAddress();



};

enum I2CState : uint8_t  {
  TRANSMIT,
  TRANSMIT_THEN_RECEIVE,
  RECEIVE,
  IDLE
};
enum I2CWriteMode : uint8_t {
  REGISTER,
  ADDRESSING,
  DATA
};

// Singleton class
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
  uint8_t _currentRegister;

  void (*_readCompleteCallback)(uint8_t *data, uint32_t length) = NULL;
  void (*_writeCompleteCallback)(uint8_t *data, uint32_t length) = NULL;
  I2Cdev();


public:
  static I2Cdev& instance();

  uint8_t getAddress();
  void setAddress(uint8_t address);

  /*!
     \brief Write a certain value to a given register
     \param data Array of data to write (if register, it should be the first element)
     \param length Length of the array of data
     \param completeCallback function called upon completion of task
     \return Nothing
  */
  void write(uint8_t data[], uint32_t length, void (*completeCallback)(uint8_t *, uint32_t) = NULL);
  void read(uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *data, uint32_t length));

  /*!
     \brief Read a certain value from a given register
     \param reg Register value to read from
     \param data Array of data to read to
     \param length Length of the array of data
     \param completeCallback function called upon completion of task
     \return Nothing
  */
  void readRegister(uint8_t reg, uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *, uint32_t));

  void callback();




  // Initialize I2C peripheral hardware
  static void initializeI2C0(uint32_t frequency);

};



extern "C" void i2c0_isr(void);



#endif
