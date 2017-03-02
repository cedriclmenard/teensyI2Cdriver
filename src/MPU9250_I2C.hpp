#ifndef MPU9250_I2C_HPP
#define MPU9250_I2C_HPP

#include "mpu9250.h"    // Registers definition
#include <stdint.h>
#include "kinetis.h"
#include "I2Cdev.hpp"

// Multiplexed MPU9250_I2C class
class MPU9250_I2C {
private:

public:
  MPU9250_I2C(uint8_t muxAddress);

};



#endif
