#ifndef MPU9250_WIRE_HPP
#define MPU9250_WIRE_HPP

#include "mpu9250.h"    // Registers definition
#include <stdint.h>
#include "kinetis.h"
#include "I2Cdev.hpp"
#include "types.h"
#include "Arduino.h"

class MPU9250_Wire_BLOCKING {
private:
  uint8_t _muxAddress;

  void setMuxToCurrentAddress();
public:

  /*!
     \brief MPU9250 blocking device constructor
     \param muxAddress Address between 0 and 7 to use for multiplexing
     This class serves as an independent device to communicate with.
  */
  MPU9250_Wire_BLOCKING(uint8_t muxAddress);

  /*!
     \brief Return the number of bytes available to read in the FIFO
     \pre The device must be initialized and connected
     \return Number of bytes available in FIFO buffer
  */
  uint32_t getNumberOfAvailableValueToRead();

  /*!
     \brief Reads the FIFO and write this data inside the given buffer
     \param dataBuffer Buffer to write the data received
     \pre The device must be initialized and connected
     \return void
  */
  void readDataFIFO(uint8_t dataBuffer[], uint32_t numberOfValues);

  /*!
     \brief Read each individual data registers on the device, without using the FIFO buffer
     \param data Data structure to write the received data to
     \pre The device must be initialized and connected
     \return void
  */
  accel_temp_gyro_t readDataRegisters();

  /*!
     \brief Initialize the device to the default clock source (internal).
     \return void
     Setup max range for accel. and gyro. data and disabled sleep. Sets FSYNC (N/I yet)
  */
  void initialize();


};


#endif
