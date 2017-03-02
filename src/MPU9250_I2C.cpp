#include "MPU9250_I2C.hpp"
#define NUM_I2C_CALLBACKS 16
#define MUXADD 0x70

I2Cdev i2c = I2Cdev::instance();

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




  I2CServingQueue& operator++() {

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
        i2c.writeRegister(dataBuffer[currentlyServing],
                          dataLength[currentlyServing], &managingCallback);
        break;
        case READ:
        i2c.readRegister(dataBuffer[currentlyServing],
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
      i2c.writeRegister(uint8_t *data, uint32_t length, optional void (*completeCallback)(uint8_t *, uint32_t))
      break;
    case I2CServingQueue::DATA:
      break;
    case I2CServingQueue::END:
      break;
  }
}
