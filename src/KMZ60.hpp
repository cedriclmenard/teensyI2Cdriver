#ifndef KMZ60_HPP
#define KMZ60_HPP

#include <stdint.h>
#include <Arduino.h>

class KMZ60 {
  uint32_t _VOUT1_PIN_TEENSY;
  uint32_t _VOUT2_PIN_TEENSY;
public:
  KMZ60(uint32_t VOUT1_PIN_TEENSY, uint32_t VOUT2_PIN_TEENSY);
  float readAngleRad();
  float readAngleDeg();

};


#endif
