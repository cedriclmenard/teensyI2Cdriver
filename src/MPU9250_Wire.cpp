#include "MPU9250_Wire.hpp"
#define MUXADD (uint8_t)0x70

static uint32_t Wire_begin_counter = 0;

// MARK: Supporting functions

uint8_t readRegister(uint8_t address,uint8_t reg) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(address, 1);
  return Wire.read();
}

void writeRegister(uint8_t address, uint8_t reg, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void readRegisterBurst(uint8_t address, uint8_t reg, uint8_t* data, uint8_t length) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(address, length);
  for (uint8_t i = 0; i < length; i++) {
    data[i] = Wire.read();
  }
}

void writeRegisterBurst(uint8_t address, uint8_t reg, uint8_t* data, uint8_t length) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, length);
  Wire.endTransmission();
}

MPU9250_Wire_BLOCKING::MPU9250_Wire_BLOCKING(uint8_t muxAddress)
{
  _muxAddress = 1 << ((muxAddress > 7) ? 7 : muxAddress);
  if (Wire_begin_counter == 0) {
    Wire.begin();
  }
  Wire_begin_counter++;
}

MPU9250_Wire_BLOCKING::~MPU9250_Wire_BLOCKING() {
  if (Wire_begin_counter == 0) {
    Wire.end();
  }
}

void MPU9250_Wire_BLOCKING::setMuxToCurrentAddress() {
  Wire.beginTransmission(MUXADD);
  Wire.write(_muxAddress);
  Wire.endTransmission();
}

uint32_t MPU9250_Wire_BLOCKING::getNumberOfAvailableValueToRead(){
  setMuxToCurrentAddress();
  // halfword_t readBuf;
  // readBuf.l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_COUNTL);
  // readBuf.h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_COUNTH);
  uint8_t lb = readRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_COUNTL);
  uint8_t hb = readRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_COUNTH);
  Serial.println((hb << 8) | lb);
  return (hb << 8) | lb;
  //return (uint32_t) ((readBuf.h << 8) | readBuf.l );
}

void MPU9250_Wire_BLOCKING::readDataFIFO(uint8_t dataBuffer[], uint32_t numberOfValues) {
  setMuxToCurrentAddress();
  readRegisterBurst(MPU9250_I2C_ADDRESS, MPU9250_FIFO_R_W, dataBuffer, numberOfValues);
}

accel_temp_gyro_t MPU9250_Wire_BLOCKING::readDataFIFOBatch() {
  accel_temp_gyro_t data;
  setMuxToCurrentAddress();
  readRegisterBurst(MPU9250_I2C_ADDRESS, MPU9250_FIFO_R_W, data.b, 14); // 14 values to read
  return data;
}

accel_temp_gyro_t MPU9250_Wire_BLOCKING::readDataRegisters() {
  setMuxToCurrentAddress();
  accel_temp_gyro_t data;
  data.reg.x_accel_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_XOUT_H);
  data.reg.x_accel_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_XOUT_L);
  data.reg.y_accel_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_YOUT_H);
  data.reg.y_accel_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_YOUT_L);
  data.reg.z_accel_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_ZOUT_H);
  data.reg.z_accel_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_ZOUT_L);
  data.reg.t_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_TEMP_OUT_H);
  data.reg.t_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_TEMP_OUT_L);
  data.reg.x_gyro_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_XOUT_H);
  data.reg.x_gyro_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_XOUT_L);
  data.reg.y_gyro_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_YOUT_H);
  data.reg.y_gyro_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_YOUT_L);
  data.reg.z_gyro_h = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_ZOUT_H);
  data.reg.z_gyro_l = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_ZOUT_L);
  return data;
}


void MPU9250_Wire_BLOCKING::initialize() {
  setMuxToCurrentAddress();
  // Set clock source and sleep disabled -------
  uint8_t regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_PWR_MGMT_1);
  //uint8_t regVal = _dev.readRegister(0x3B);
  regVal &= ~0x7; // Mask for 3 first bits, set clock source to internal
  regVal &= ~0x40; // Mask for bit 6, sleep disabled

  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_PWR_MGMT_1, regVal);

  // Enable low pass filter
  regVal = readRegister(MPU9250_I2C_ADDRESS,MPU9250_CONFIG);
  regVal |= 3;
  writeRegister(MPU9250_I2C_ADDRESS,MPU9250_CONFIG, regVal);

  // Set gyro range -------
  regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_CONFIG);

  regVal |= 0x18; // Mask for bits 3:4, set to full scale (+2000 dps)

  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_GYRO_CONFIG, regVal);

  // Set accel range -------
  regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_CONFIG);

  regVal |= 0x18; // Mask for bits 3:4, set to full scale (16g)

  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_ACCEL_CONFIG, regVal);



  // Outputs 500Hz
  writeRegister(MPU9250_I2C_ADDRESS,MPU9250_SMPLRT_DIV, 15);

  // Set FSYNC
  regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_SMPLRT_DIV);
  regVal |= 1 << 3; // Set FSYNC to temperature output
  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_SMPLRT_DIV, regVal);

  // Enable FIFO
  regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_CONFIG);
  regVal |= 1 << 7; // Set FIFO_MODE to not overwrite
  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_CONFIG, regVal);

  regVal = readRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_EN);
  regVal |= 0xFF << 3; // Set FIFO to save accel, gyro and temp
  writeRegister(MPU9250_I2C_ADDRESS, MPU9250_FIFO_EN, regVal);
}
