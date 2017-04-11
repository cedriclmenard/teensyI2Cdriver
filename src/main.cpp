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

// MARK: Supporting functions
void sendFSYNC(uint8_t fsyncPin) {
  digitalWrite(uint8_t fsyncPin, HIGH);
  digitalWrite(uint8_t fsyncPin, LOW);
}

//MPU9250_I2C_BLOCKING dev = MPU9250_I2C_BLOCKING(0);
//BlockingI2Cdev dev = BlockingI2Cdev(MPU9250_I2C_ADDRESS);

//MPU9250_Wire_BLOCKING dev = MPU9250_Wire_BLOCKING(0);

// Declaration des 8 imus
MPU9250_Wire_BLOCKING imus[8] = {MPU9250_Wire_BLOCKING(0),
  MPU9250_Wire_BLOCKING(1), MPU9250_Wire_BLOCKING(2), MPU9250_Wire_BLOCKING(3),
MPU9250_Wire_BLOCKING(4), MPU9250_Wire_BLOCKING(5), MPU9250_Wire_BLOCKING(6),
MPU9250_Wire_BLOCKING(7)};

uint32_t nbOfValues[8]; // One number per IMU
uint32_t maxNbVal; // Sum of number of values to read. (Worst case)
uint32_t counterValStored[8]; // One number per IMU
accel_temp_gyro_t data;

uint32_t VOUT1_PIN_TEENSY = 14;
uint32_t VOUT2_PIN_TEENSY = 15;

KMZ60 kmz = KMZ60(VOUT1_PIN_TEENSY, VOUT2_PIN_TEENSY);
double angleAileronRad;

//KMZ60 kmz = KMZ60(14,15);

void setup()
{
  // initialize LED digital pin as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  // I2Cdev::initializeI2C0(100000);

  // Initialisation des 8 imus
  for ( int i = 0; i < 8; i = i + 1  ) {
    imus[i].initialize();
  }

  Serial.begin(38400);

  delay(3000);
  //dev.initialize();
  //Serial.println("Hello from the otter slide");

}


void loop()
{
  // Programmation du main par Vincent

  // 1: Gathering values for 8 IMUs
  // WTF: What size for arrays containing accel et gyros?
  maxNbVal = 0 ;
  for ( int i = 0; i < 8; i = i + 1  ) {
    nbOfValues[i] = imus[i].getNumberOfAvailableValueToRead();
    maxNbVal = (maxNbVal<nbOfValues[i])?nbOfValues[i]:maxNbVal; ; // Max
    counterValStored[i] = -1; // Reset of counter for each IMU
  }
  // For now, worst case considered (All values in register were taken during
  // the FSYNC). Could be optimized for memory or speed if required. Better
  // than resizing everytime we add a value?
  // One colomn for each IMU
  double xAccel [maxNbVal][8];
  double yAccel [maxNbVal][8];
  double zAccel [maxNbVal][8];
  double xGyro [maxNbVal][8];
  double yGyro [maxNbVal][8];
  double zGyro [maxNbVal][8];
  for ( uint i = 0; i < 8; i += 1  ) {
      Serial.print("Reading Imu: ");
      Serial.print(i);
      Serial.println();
      // For the number of values to read for each IMU
      for (uint j = 0; j < nbOfValues[i]; j += 1){
        data = imus[i].readDataFIFOBatch();
        // WTF: data.value ou data.reg?
        if (data.value.x_accel & 0x1){ //if value stored while FSYNC on
          counterValStored[i] += 1; // Add a value reading to that IMU
          xAccel[counterValStored[i]][i] = ((double)data.value.x_accel)/32768.0*16;
          yAccel[counterValStored[i]][i] = ((double)data.value.y_accel)/32768.0*16;
          zAccel[counterValStored[i]][i] = ((double)data.value.z_accel)/32768.0*16;
          xGyro[counterValStored[i]][i] = ((double)data.value.x_gyro)/32768.0*2000;
          yGyro[counterValStored[i]][i] = ((double)data.value.y_gyro)/32768.0*2000;
          zGyro[counterValStored[i]][i] = ((double)data.value.z_gyro)/32768.0*2000;
          // TEMPORARY PRINT DEBUG PURPOUSE
          Serial.print("x = ");
          Serial.print(xAccel[counterValStored[i]][i]);
          Serial.print("  y = ");
          Serial.print(yAccel[counterValStored[i]][i]);
          Serial.print("  z = ");
          Serial.print(zAccel[counterValStored[i]][i]);
          Serial.print("  x Gyro = ");
          Serial.print(xGyro[counterValStored[i]][i]);
          Serial.print("  y Gyro = ");
          Serial.print(yGyro[counterValStored[i]][i]);
          Serial.print("  z Gyro = ");
          Serial.print(zGyro[counterValStored[i]][i]);
        } // end if FSYNCed
      } // end for j
    } // end for i

    // 2: Synchronizing readings with FSYNC
    // WTF: Doc MPU-9250 p. 13, comment on utilise FSYNC? EXT_SYNC_SET[2:0] (Adresse hexa = 1A)?

    // 3: Mesuring deformation with HX711
    // TO DO Voir avec Ced pour avoir la deformation

    // 4: Mesuring Aileron position with KMZ60
    angleAileronRad = kmz.readAngleRad();

    // 5: Security check
    // TO DO Implement security criterions

    // 6: Triggering security devices (If security check failed)
    // WTF: HOW?

    // 7: Connecting with servomotor

    // 8: Adjusting Angle of Attack of servomotor, potentiometer?

    // Delay could be adjusted/removed, for speed considerations
    delay(1000);
  }



  // KEPT OLD MAIN FOR REFERENCING IF NEEDED, WILL BE CLEARED ONCE COMPLETED

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

  //accel_temp_gyro_t val = imus[i].readDataRegisters();
  //Serial.print("x = ");
  //Serial.print(((double)val.value.x_accel)/32768.0*16);
  //Serial.print(", y = ");
  //Serial.print(((double)val.value.y_accel)/32768.0*16);
  //Serial.print(", z = ");
  //Serial.print(((double)val.value.z_accel)/32768.0*16);
  //Serial.print(", x gyro = ");
  //Serial.print(((double)val.value.x_gyro)/32768.0*2000);
  //Serial.print(", y gyro = ");
  //Serial.print(((double)val.value.y_gyro)/32768.0*2000);
  //Serial.print(", z gyro = ");
  //Serial.print(((double)val.value.z_gyro)/32768.0*2000);
  //Serial.println();


  //accel_temp_gyro_t val = dev.readDataRegisters();
  //Serial.print("x = ");
  //Serial.print(((double)val.value.x_accel)/32768.0*16);
  //Serial.print(", y = ");
  //Serial.print(((double)val.value.y_accel)/32768.0*16);
  //Serial.print(", z = ");
  //Serial.print(((double)val.value.z_accel)/32768.0*16);
  //Serial.print(", x gyro = ");
  //Serial.print(((double)val.value.x_gyro)/32768.0*2000);
  //Serial.print(", y gyro = ");
  //Serial.print(((double)val.value.y_gyro)/32768.0*2000);
  //Serial.print(", z gyro = ");
  //Serial.print(((double)val.value.z_gyro)/32768.0*2000);
  //Serial.println();
  //Serial.print(" ");
  //Serial.println(val[1]);
  //delay(1000);
