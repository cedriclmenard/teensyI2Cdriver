#include "MPU9250_I2C.hpp"
#define NUM_I2C_CALLBACKS 16
#define MUXADD (uint8_t)0x70


//static I2Cdev i2c = I2Cdev::instance();

void managingCallback(uint8_t *data, uint32_t length);

// MARK: Multiple call implementation to write/read
struct I2CServingQueue {
  // Index of c
  uint8_t currentlyServing = 255;
  uint8_t remainingToServe = 0;

  // Data pointer buffer (each pointer to data from requests goes in this)
  uint8_t* dataBuffer[NUM_I2C_CALLBACKS] = {NULL};
  uint32_t dataLength[NUM_I2C_CALLBACKS] = {0};

  // Request type (selects which function to call) buffer
  enum RequestType {WRITE, READ};
  RequestType reqTypeBuffer[NUM_I2C_CALLBACKS];

  // Callback fct pointer buffer (after operation has finished)
  void (*i2c0_callbacks[NUM_I2C_CALLBACKS])(void) = {NULL};

  // Mux addressing
  uint8_t muxAddresses[NUM_I2C_CALLBACKS];
  enum OperationState {MUXADDR, DATA, END};
  OperationState opState = MUXADDR;

  // Read operation array of register address
  uint8_t regForRead[NUM_I2C_CALLBACKS] = {0};




  I2CServingQueue& operator++() {
    static I2Cdev i2c = I2Cdev::instance();

    #ifdef DEBUG
    i2c0_callbacks[currentlyServing] = NULL;
    dataBuffer[currentlyServing] = NULL;
    #endif


    currentlyServing++;
    if (currentlyServing >= NUM_I2C_CALLBACKS) {
      // Rollback
      currentlyServing = 0;
    }
    if (remainingToServe > 0) {
      remainingToServe--;
      switch (reqTypeBuffer[currentlyServing]) {
        case WRITE:
        i2c.write(dataBuffer[currentlyServing],
                          dataLength[currentlyServing], &managingCallback);
        break;
        case READ:
        i2c.readRegister(regForRead[currentlyServing],dataBuffer[currentlyServing],
                         dataLength[currentlyServing], &managingCallback);
        break;
      }
    }

    return *this;
  }

} i2c0ServingQueue;

void managingCallback(uint8_t *data, uint32_t length) {
  switch (i2c0ServingQueue.opState) {
    case I2CServingQueue::MUXADDR:
    // TODO: Implement this
      //i2c.write()
      break;
    case I2CServingQueue::DATA:
      break;
    case I2CServingQueue::END:
      break;
  }
}


// MARK: BlockingI2Cdev

MPU9250_I2C_BLOCKING::MPU9250_I2C_BLOCKING(uint8_t muxAddress){
  _muxAddress = 1 << ((muxAddress > 7) ? 7 : muxAddress);
}

uint32_t MPU9250_I2C_BLOCKING::getNumberOfAvailableValueToRead(){
  setMuxToCurrentAddress();
  halfword_t readBuf;
  readBuf.h = _dev.readRegister(MPU9250_FIFO_COUNTH);
  readBuf.l = _dev.readRegister(MPU9250_FIFO_COUNTL);
  return (uint32_t) readBuf.hw;
}

void MPU9250_I2C_BLOCKING::readDataFIFO(uint8_t dataBuffer[], uint32_t numberOfValues) {
  setMuxToCurrentAddress();
  _dev.readRegisterBurst(MPU9250_FIFO_R_W, dataBuffer, numberOfValues);
}

accel_temp_gyro_t MPU9250_I2C_BLOCKING::readDataRegisters() {
  setMuxToCurrentAddress();
  accel_temp_gyro_t data;
  data.reg.x_accel_h = _dev.readRegister(MPU9250_ACCEL_XOUT_H);
  data.reg.x_accel_l = _dev.readRegister(MPU9250_ACCEL_XOUT_L);
  data.reg.y_accel_h = _dev.readRegister(MPU9250_ACCEL_YOUT_H);
  data.reg.y_accel_l = _dev.readRegister(MPU9250_ACCEL_YOUT_L);
  data.reg.z_accel_h = _dev.readRegister(MPU9250_ACCEL_ZOUT_H);
  data.reg.z_accel_l = _dev.readRegister(MPU9250_ACCEL_ZOUT_L);
  data.reg.t_h = _dev.readRegister(MPU9250_TEMP_OUT_H);
  data.reg.t_l = _dev.readRegister(MPU9250_TEMP_OUT_L);
  data.reg.x_gyro_h = _dev.readRegister(MPU9250_GYRO_XOUT_H);
  data.reg.x_gyro_l = _dev.readRegister(MPU9250_GYRO_XOUT_L);
  data.reg.y_gyro_h = _dev.readRegister(MPU9250_GYRO_YOUT_H);
  data.reg.y_gyro_l = _dev.readRegister(MPU9250_GYRO_YOUT_L);
  data.reg.z_gyro_h = _dev.readRegister(MPU9250_GYRO_ZOUT_H);
  data.reg.z_gyro_l = _dev.readRegister(MPU9250_GYRO_ZOUT_L);
  return data;
}


void MPU9250_I2C_BLOCKING::initialize() {
  setMuxToCurrentAddress();
  // Set clock source and sleep disabled -------
  uint8_t regVal = _dev.readRegister(MPU9250_PWR_MGMT_1);
  //uint8_t regVal = _dev.readRegister(0x3B);
  regVal &= ~0x7; // Mask for 3 first bits, set clock source to internal
  regVal &= ~0x40; // Mask for bit 6, sleep disabled

  _dev.writeRegister(MPU9250_PWR_MGMT_1, regVal);

  // Set gyro range -------
  regVal = _dev.readRegister(MPU9250_GYRO_CONFIG);

  regVal |= 0x18; // Mask for bits 3:4, set to full scale (+2000 dps)

  _dev.writeRegister(MPU9250_GYRO_CONFIG, regVal);

  // Set accel range -------
  regVal = _dev.readRegister(MPU9250_ACCEL_CONFIG);

  regVal |= 0x18; // Mask for bits 3:4, set to full scale (16g)

  _dev.writeRegister(MPU9250_ACCEL_CONFIG, regVal);

  // Set FSYNC
  // TODO: Set FSYNC
  // TODO: Enable FIFO
}

void MPU9250_I2C_BLOCKING::setMuxToCurrentAddress() {
  _dev.setAddress(MUXADD);
  _dev.write(_muxAddress);
  _dev.setAddress(MPU9250_I2C_ADDRESS);
}
