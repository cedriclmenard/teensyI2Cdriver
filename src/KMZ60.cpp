#include "KMZ60.hpp"

#define RAD2DEG 57.2957795131

KMZ60::KMZ60(uint32_t VOUT1_PIN_TEENSY, uint32_t VOUT2_PIN_TEENSY) :
_VOUT1_PIN_TEENSY(VOUT1_PIN_TEENSY), _VOUT2_PIN_TEENSY(VOUT2_PIN_TEENSY)
{

}

double KMZ60::readAngleRad() {
  double cos_c = ((double)analogRead(_VOUT1_PIN_TEENSY))/1023 - 0.5;
  double sin_c = ((double)analogRead(_VOUT2_PIN_TEENSY))/1023 - 0.5;
  return atan(sin_c/cos_c);
}

double KMZ60::readAngleDeg() {
  double cos_c = ((double)analogRead(_VOUT1_PIN_TEENSY))/1023 - 0.5;
  double sin_c = ((double)analogRead(_VOUT2_PIN_TEENSY))/1023 - 0.5;
  return atan(sin_c/cos_c)*RAD2DEG;
}
