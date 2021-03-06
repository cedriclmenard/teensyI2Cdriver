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
#include <stdint.h>
#include "HX711.hpp"
//#define DEBUG

//#define PRINTHUMAN // Comment for machine format, uncomment for text

// MARK: Supporting functions
void sendFSYNC(uint8_t thisFsyncPin) {
  digitalWrite(thisFsyncPin, HIGH);
  digitalWrite(thisFsyncPin, LOW);
}

void fireSecurity(uint8_t thisSecurityPin){
  digitalWrite(thisSecurityPin, HIGH);
  Serial.println();
  Serial.print("SECURITY FIRED");
  digitalWrite(thisSecurityPin, LOW);
  Serial.println();
}

void printNewLineMachine(uint32_t thisTimeRead) {
  Serial.println();
  Serial.print(thisTimeRead);
  Serial.print(",");
}

void printNewLineHuman(uint32_t thisTimeRead) {
  Serial.println();
  Serial.print("Temps de lecture (ms) = ");
  Serial.print(thisTimeRead);
}

void printAccelsMachine(uint i, float ax, float ay, float az, float gx, float gy, float gz){
  Serial.print(i);
  Serial.print(",");
  Serial.print(ax);
  Serial.print(",");
  Serial.print(ay);
  Serial.print(",");
  Serial.print(az);
  Serial.print(",");
  Serial.print(gx);
  Serial.print(",");
  Serial.print(gy);
  Serial.print(",");
  Serial.print(gz);
  Serial.print(",");
}

void printAccelsHuman(uint i, float mag, float ax, float ay, float az, float gx, float gy, float gz){
  Serial.println();
  Serial.print("IMU = ");
  Serial.print(i);
  Serial.println();
  Serial.print("Acceleration Magnitude = ");
  Serial.print(mag);
  Serial.print("  x = ");
  Serial.print(ax);
  Serial.print("  y = ");
  Serial.print(ay);
  Serial.print("  z = ");
  Serial.print(az);
  Serial.print("  x Gyro = ");
  Serial.print(gx);
  Serial.print("  y Gyro = ");
  Serial.print(gy);
  Serial.print("  z Gyro = ");
  Serial.print(gz);
}

void printAngleMachine(float ang) {
Serial.print(ang);
}

void printAngleHuman(float ang) {
Serial.println();
Serial.print("Angle aileron (deg) = ");
Serial.print(ang);
}



//MPU9250_I2C_BLOCKING dev = MPU9250_I2C_BLOCKING(0);
//BlockingI2Cdev dev = BlockingI2Cdev(MPU9250_I2C_ADDRESS);

//MPU9250_Wire_BLOCKING dev = MPU9250_Wire_BLOCKING(0);

// Declaration des 8 imus
// TEST!
MPU9250_Wire_BLOCKING imus[8] = {MPU9250_Wire_BLOCKING(0),
  MPU9250_Wire_BLOCKING(1), MPU9250_Wire_BLOCKING(2), MPU9250_Wire_BLOCKING(3),
MPU9250_Wire_BLOCKING(4), MPU9250_Wire_BLOCKING(5), MPU9250_Wire_BLOCKING(6),
MPU9250_Wire_BLOCKING(7)};

//MPU9250_Wire_BLOCKING imus[1] = {MPU9250_Wire_BLOCKING(5)};

uint32_t nbOfValues[8]; // One number per IMU
accel_temp_gyro_t data;
uint32_t timeRead;

uint32_t VOUT1_PIN_TEENSY = 14;
uint32_t VOUT2_PIN_TEENSY = 15;
uint8_t fsyncPin = 5; // Pin for fSYNC
uint8_t securityPin = 26; // Pin to fire security devices

KMZ60 kmz = KMZ60(VOUT1_PIN_TEENSY, VOUT2_PIN_TEENSY);
float angleAileronDeg;

// Need to keep 3 values for each of the 8 IMUS for security check.
float AccelsMagSquare [8][3]; // Security only needs magnitude square
float AccelsMagPre [8][2]; // To ensure maximum, we need to store 2 previous values
float xAccel; // Each IMU read is stored to calculate Magnitude and print
float yAccel;
float zAccel;
float xGyro; // Same as accels
float yGyro;
float zGyro;
float newAccelMagSq; //new magnitude of acceleration square
//int32_t hx711Value; //

// WTF: SECUTIRY !!!! SEE VALUES BELOW. WHERE SHOULD WE CHECK?
// Square of Absolute accel that should not be exceeded by any imu (note Square
// because root square would be long to compute for nothing, and avoid sign check)
float accelMaxSquare = 1.5;
// 4 consecutive increasing maximum should not trigger if last accel is below a
// certain threshold
float accelMinSecutiry = 1.1;
//int32_t hx711ValMax = 100000; // WTF: NEEDS CALIBRATION

uint32_t loopDelay = 1000; // milisecs

//KMZ60 kmz = KMZ60(14,15);

// MARK: Initialize HX711
//HX711 hx711Dev = HX711(&GPIOE_PDOR, 26, &GPIOA_PDIR,
//  5, X128);

void setup()
{
  Serial.begin(38400);
  Serial.print("Initialised");
  Serial.println();
  // MARK: HX711 pin init
  GPIOE_PDDR &= ~(1<<26);
  GPIOA_PDDR |= (1<<5);

  // MARK: Init security pin
  pinMode(securityPin, OUTPUT);

  // initialize LED digital pin as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  // I2Cdev::initializeI2C0(100000);

  // TEST! Change i 1 or 8
  // Initialisation des 8 imus
  for ( uint i = 0; i < 8; i = i + 1  ) {
    imus[i].initialize();
    // Needs to initialize all values in AccelsMagSquare, to ensure the shifting
    // of values when reading doesn't access residual memory
    for ( uint j = 0; j < 2; j = j + 1  ) { // 2 latest values
      AccelsMagSquare[i][j] = 0.0;
      AccelsMagPre [i][j] = 0.0;
    }// end for 3 latest values
    AccelsMagSquare[i][2] = 0.0; // AccelMagSquare needs one more value than AccelsMagPre
  }// end for 8 IMUS
  //pinMode(securityPin,OUTPUT);
  digitalWrite(securityPin, LOW);
  //dev.initialize();
  //Serial.println("Hello from the otter slide");
}

#ifdef DEBUG
void loop() {
  writeToMainServo();
  delay(5000);
}
#endif


#ifndef DEBUG
void loop()
{



  // Programmation du main par Vincent

  // 1: Gathering values for 8 IMUs
  // for each IMU, get number of values to read
  // REMOVING READFIFO
  // for ( int i = 0; i < 8; i = i + 1  ) {
  //   // divided by 14, because 14 bites per complete reads (2 for each accel, each gyro and temperature)
  //   nbOfValues[i] = (uint32_t) (imus[i].getNumberOfAvailableValueToRead())/14.0;
  //   Serial.println();
  //   Serial.print("NOT DIVIDED NUMBER OF VALUES = ");
  //   Serial.print(imus[i].getNumberOfAvailableValueToRead());
  // }

  // TEST! Change i 1 or 8
  // for each IMU
  // Serial.println();

  timeRead = micros();
  #ifdef PRINTHUMAN
    printNewLineHuman(timeRead);
  #else
    printNewLineMachine(timeRead);
  #endif
  for ( uint i = 0; i < 8; i += 1  ) {

      // Serial.print(i);
      // Serial.print(",");


      // REMOVING READFIFO
      // // For the number of values to read for each IMU
      // for (uint nbVal = 0; nbVal < nbOfValues[i]; nbVal += 1){
      //   data = imus[i].readDataFIFOBatch(); // if not SYNCED, no interest
      //   if (data.value.x_accel & 0x1){ //if value stored while FSYNC on

          data = imus[i].readDataRegisters();

          xAccel = ((float)data.value.x_accel)/32768.0*16; // new x
          yAccel = ((float)data.value.y_accel)/32768.0*16; // new y
          zAccel = ((float)data.value.z_accel)/32768.0*16; // new z
          xGyro = ((float)data.value.x_gyro)/32768.0*2000;
          yGyro = ((float)data.value.y_gyro)/32768.0*2000;
          zGyro = ((float)data.value.z_gyro)/32768.0*2000;
          newAccelMagSq = xAccel*xAccel+yAccel*yAccel+zAccel*zAccel;
          //hx711Value = abs(hx711Dev.blockingRead()); // NEEDS CALIBRATION

          // Security: if newAccel>accelMax, or Stain > CalibratedMaxStrain or (4 consecutive increasing maximums, with the newest > accelMinSecutiry)
          //|| (hx711Value > hx711ValMax)
          if ((newAccelMagSq>accelMaxSquare) || ((AccelsMagSquare [i][2]<AccelsMagSquare [i][1])&(AccelsMagSquare [i][1]<AccelsMagSquare [i][0])&(AccelsMagSquare [i][0]<newAccelMagSq)&(newAccelMagSq>accelMinSecutiry))){
            fireSecurity(securityPin);
          }

          // if it's a maximum
          // shifting values in AccelMagSquare & keeping order (Overwriting oldest)
          if ((AccelsMagPre [i][1]<AccelsMagPre [i][0])&&(newAccelMagSq<AccelsMagPre [i][0])){
            for ( int j = 1; j > -1; j -= 1  ) { // 2 latest values (j already used)
              AccelsMagSquare [i][j+1] = AccelsMagSquare [i][j];
            } // end for 2 latest values
            AccelsMagSquare [i][0] = newAccelMagSq; // new Accel Mag
          }
          AccelsMagPre [i][1] = AccelsMagPre [i][0];
          AccelsMagPre [i][0] = newAccelMagSq;

          #ifdef PRINTHUMAN
            printAccelsHuman(i,newAccelMagSq,xAccel,yAccel,zAccel,xGyro,yGyro,zGyro);
          #else
            printAccelsMachine(i,xAccel,yAccel,zAccel,xGyro,yGyro,zGyro);
          #endif



        } // end if FSYNCed

      // REMOVING READFIFO
    //   } // end for nbVal
    //   // Serial.println();
    //   // Serial.print("NUMBER OF VALUES ");
    //   // Serial.print(nbOfValues[i]);
    // } // end for i (8 IMUS)
    // // 2: Synchronizing readings with FSYNC
    // sendFSYNC(fsyncPin);

    // 3: Mesuring deformation with HX711
    // TO DO Voir avec Ced pour avoir la deformation

    // 4: Mesuring Aileron position with KMZ60
    angleAileronDeg = kmz.readAngleDeg();


    #ifdef PRINTHUMAN
      printAngleHuman(angleAileronDeg);
    #else
      printAngleMachine(angleAileronDeg);
    #endif




    // 5: Security check
    // IMPLEMENTED, check occurs everytime a new value is read

    // 6: Triggering security devices (If security check failed)
    // DONE to test

    // 7: Connecting with servomotor?

    // 8: Adjusting Angle of Attack of servomotor, potentiometer?

    // Delay could be adjusted/removed, for speed considerations
    //delay(loopDelay);
  }
#endif


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
