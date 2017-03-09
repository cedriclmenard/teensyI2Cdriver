#ifndef HX711_HPP
#define HX711_HPP

#include <stdint.h>
#include "kinetis.h"

enum HX711Gain : uint8_t {
  X128 = 25,
  X64 = 26,
  X32 = 27
};

class HX711 {
private:
  uint32_t* _DATAOUTPort;
  uint32_t* _SCKPort;
  uint32_t _DATAOUTPin;
  uint32_t _SCKPin;
  HX711Gain _gain;
public:
  HX711(uint32_t* DATAOUTPort, uint32_t DATAOUTPin, uint32_t* SCKPort,
    uint32_t SCKPin, HX711Gain gain);
  ~HX711();
  uint32_t blockingRead();
};

#endif
