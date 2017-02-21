#ifndef HX711_HPP
#define HX711_HPP

#include <stdint.h>
#include "kinetis.h"

enum HX711Gain : uint8_t {
  X128 = 128,
  X64 = 64,
  X32 = 32
};

class HX711 {
private:

public:
  HX711(uint32_t& DATAOUTPin, uint32_t& SCKPin, HX711Gain gain);
  ~HX711();
};

#endif
