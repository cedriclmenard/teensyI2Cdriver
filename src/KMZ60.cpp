#include "KMZ60.hpp"

#define RAD2DEG 57.2957795131

KMZ60::KMZ60(uint32_t VOUT1_PIN_TEENSY, uint32_t VOUT2_PIN_TEENSY) :
_VOUT1_PIN_TEENSY(VOUT1_PIN_TEENSY), _VOUT2_PIN_TEENSY(VOUT2_PIN_TEENSY)
{

}

float KMZ60::readAngleRad() {
  float cos_c = ((float)analogRead(_VOUT1_PIN_TEENSY))/1023 - 0.5;
  float sin_c = ((float)analogRead(_VOUT2_PIN_TEENSY))/1023 - 0.5;
  return atan(sin_c/cos_c);
}

float KMZ60::readAngleDeg() {
  float cos_c = ((float)analogRead(_VOUT1_PIN_TEENSY))/1023 - 0.5;
  float sin_c = ((float)analogRead(_VOUT2_PIN_TEENSY))/1023 - 0.5;
  return atan(sin_c/cos_c)*RAD2DEG;
}
