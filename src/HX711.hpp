#ifndef HX711_HPP
#define HX711_HPP

#include <stdint.h>
#include "kinetis.h"

/*!
   \brief HX711 Gain enum
*/
enum HX711Gain : uint8_t {
  X128 = 25,
  X64 = 26,
  X32 = 27
};

/*!
   \brief HX711 DTO class
*/
class HX711 {
private:
  uint32_t* _DATAOUTPort;
  uint32_t* _SCKPort;
  uint32_t _DATAOUTPin;
  uint32_t _SCKPin;
  HX711Gain _gain;
public:

  /*!
     \brief Creates a HX711 DT object to communicate with a HX711 on given port and pins
     \param DATAOUTPort Port (uint32_t pointer to the port) used as DATAOUT
     \param DATAOUTPin Pin number corresponding to the pin used for DATAOUT
     \param SCKPort Port (uint32_t pointer to the port) used as SCK
     \param SCKPin Pin number corresponding to the pin used for SCK
     \param gain Gain value to be used for this HX711 (HX711 gain enum)
     \pre Ports and pins must already be configured as GPIO input (for DATAOUT) and output (for SCK)
  */
  HX711(uint32_t* DATAOUTPort, uint32_t DATAOUTPin, uint32_t* SCKPort,
    uint32_t SCKPin, HX711Gain gain);
  ~HX711();

  /*!
     \brief Reads in a blocking manner the given HX711 device
     \return HX711 ADC value (24 bit cast into a 32 bit unsigned integer)
  */
  int32_t blockingRead();
};

#endif
