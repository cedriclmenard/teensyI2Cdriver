/**
 * First test
 */
#include "Arduino.h"
#include "I2Cdev.hpp"
#include "MPU9250_I2C.hpp"
#include "types.h"
#include <stdlib.h>

MPU9250_I2C_BLOCKING dev = MPU9250_I2C_BLOCKING(0);

void setup()
{
  // initialize LED digital pin as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  I2Cdev::initializeI2C0(400000);
  Serial.begin(9600);
  dev.initialize();

}

void loop()
{
  accel_temp_gyro_t result = dev.readDataRegisters();
  char buf[10];
  itoa(result.value.x_accel,buf,10);
  Serial.print(buf);

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, HIGH);

  // wait for a second
  //delay(1000);

  // turn the LED off by making the voltage LOW
  //digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(1000);



}
