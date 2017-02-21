#include "HX711.hpp"

HX711::HX711(uint32_t& DATAOUTPin, uint32_t& SCKPin, HX711Gain gain) {
  // Initialize PIT module clock
  SIM_SCGC6 |= SIM_SCGC6_PIT;

  // Goal is 1 MHz communication speed with the module
  // We use module 3 as to interfere only in extreme case with Teensy
  PIT_LDVAL3 = F_BUS/1000000 - 1;

  


}
