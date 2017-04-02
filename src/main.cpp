/**
 * First test
 */
#include "Arduino.h"
#include "I2Cdev.hpp"
#include "MPU9250_I2C.hpp"
#include "MPU9250_Wire.hpp"
#include "types.h"
#include <stdlib.h>
#include "KMZ60.hpp"

//MPU9250_I2C_BLOCKING dev = MPU9250_I2C_BLOCKING(0);
//BlockingI2Cdev dev = BlockingI2Cdev(MPU9250_I2C_ADDRESS);

MPU9250_Wire_BLOCKING dev = MPU9250_Wire_BLOCKING(0);

//KMZ60 kmz = KMZ60(14,15);

void setup()
{
  // initialize LED digital pin as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  // I2Cdev::initializeI2C0(100000);

  dev.initialize();
  Serial.begin(38400);

  delay(3000);
  //dev.initialize();
  //Serial.println("Hello from the otter slide");

}


void loop()
{
  // uint32_t vout1 = analogRead(14);
  // uint32_t vout2 = analogRead(15);
  // uint32_t vtemp = analogRead(16);
  // delay(50);
  // vout1 += analogRead(14);
  // vout2 += analogRead(15);
  // vtemp += analogRead(16);
  // delay(50);
  // vout1 += analogRead(14);
  // vout2 += analogRead(15);
  // vtemp += analogRead(16);
  // delay(50);
  // vout1 += analogRead(14);
  // vout2 += analogRead(15);
  // vtemp += analogRead(16);
  // delay(50);
  // vout1 += analogRead(14);
  // vout2 += analogRead(15);
  // vtemp += analogRead(16);
  // delay(50);
  // vout1 += analogRead(14);
  // vout2 += analogRead(15);
  // vtemp += analogRead(16);
  // vout1 = vout1/6;
  // vout2 = vout2/6;
  // vtemp = vtemp/6;
  //
  // double v1 = (double)vout1/1024 - 0.5;
  // double v2 = (double)vout2/1024 - 0.5;
  // double a = atan(v2/v1)/3.141596*180;

  // Serial.print("Alpha = ");
  // Serial.print(kmz.readAngleDeg());
  // //Serial.print(v1);
  // //Serial.print(", Vout2 = ");
  // //Serial.print(v2);
  // Serial.println("°");
  // //Serial.printf("Vout1 = %1.5f, Vout2 = %1.5f, Vtemp = %i\n", v1, v2, vtemp);
  // delay(500);

  //accel_temp_gyro_t data = dev.readDataRegisters();
  //Serial.println(data.value.x_accel);
  // dev.setMuxToCurrentAddress();
  // uint8_t val[2];
  // val[0] = readRegister(0x68, 0x3C);
  // val[1] = readRegister(0x68, 0x3B);
  // int16_t res = ((uint16_t) val[1] << 8) | val[0];
  // Serial.println(res);
  // int16_t r = dev.readDataRegisters().value.x_accel;
  accel_temp_gyro_t val = dev.readDataRegisters();
  Serial.print("x = ");
  Serial.print(((double)val.value.x_accel)/32768.0*16);
  Serial.print(", y = ");
  Serial.print(((double)val.value.y_accel)/32768.0*16);
  Serial.print(", z = ");
  Serial.print(((double)val.value.z_accel)/32768.0*16);
  Serial.print(", x gyro = ");
  Serial.print(((double)val.value.x_gyro)/32768.0*2000);
  Serial.print(", y gyro = ");
  Serial.print(((double)val.value.y_gyro)/32768.0*2000);
  Serial.print(", z gyro = ");
  Serial.print(((double)val.value.z_gyro)/32768.0*2000);
  Serial.println();
  //Serial.print(" ");
  //Serial.println(val[1]);
  delay(1000);
}
