/**
 * First test
 */
#include "Arduino.h"
#include "I2Cdev.hpp"
#include "MPU9250_I2C.hpp"
#include "types.h"
#include <stdlib.h>

//MPU9250_I2C_BLOCKING dev = MPU9250_I2C_BLOCKING(0);
BlockingI2Cdev dev = BlockingI2Cdev(MPU9250_I2C_ADDRESS);

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  I2Cdev::initializeI2C0(100000);


  Serial.begin(38400);

  delay(3000);
  //dev.initialize();
  //Serial.println("Hello from the otter slide");

}


void loop()
{
  //digitalWrite(LED_BUILTIN,HIGH);
  //Serial.println("Hello");
  //accel_temp_gyro_t result = dev.readDataRegisters();
  //int x_l = dev.readRegister(0x3C);
  dev.setAddress(0x70);  // MUX address
  dev.write(1);
  Serial.println("b");
  dev.setAddress(MPU9250_I2C_ADDRESS);
  uint8_t x_l = dev.readRegister((uint8_t)0x3C);
  Serial.println(x_l);
  //char buf[10];
  //itoa(result.value.x_accel,buf,10);
  //Serial.println(result.value.x_accel);

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, HIGH);

  // wait for a second
  //delay(1000);

  // turn the LED off by making the voltage LOW
  //digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(1000);



}
