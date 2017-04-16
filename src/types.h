#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Types used for I2C communications

// Byte
typedef uint8_t byte_t;

union halfword_t
{
  uint8_t b[2];

  uint8_t l;
  uint8_t h;


  uint16_t hw;
};

union word_t
{
  uint8_t b[4];

  uint8_t ll;
  uint8_t l;
  uint8_t h;
  uint8_t hh;



  uint16_t hw[2];

  uint16_t hw_l;
  uint16_t hw_h;

  uint32_t w;
};


// MARK: Types used for MPU9250

union accel_temp_gyro_t
{
  uint8_t b[14];
  __attribute__((packed)) struct
  {
    uint8_t x_accel_l;
    uint8_t x_accel_h;
    uint8_t y_accel_l;
    uint8_t y_accel_h;
    uint8_t z_accel_l;
    uint8_t z_accel_h;
    uint8_t t_l;
    uint8_t t_h;
    uint8_t x_gyro_l;
    uint8_t x_gyro_h;
    uint8_t y_gyro_l;
    uint8_t y_gyro_h;
    uint8_t z_gyro_l;
    uint8_t z_gyro_h;
  } reg;
  __attribute__((packed)) struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};

union accel_gyro_t
{
  uint8_t b[12];
  __attribute__((packed)) struct
  {
    uint8_t x_accel_l;
    uint8_t x_accel_h;
    uint8_t y_accel_l;
    uint8_t y_accel_h;
    uint8_t z_accel_l;
    uint8_t z_accel_h;
    uint8_t x_gyro_l;
    uint8_t x_gyro_h;
    uint8_t y_gyro_l;
    uint8_t y_gyro_h;
    uint8_t z_gyro_l;
    uint8_t z_gyro_h;
  } reg;
  __attribute__((packed)) struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};


#endif
