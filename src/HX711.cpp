#include "HX711.hpp"

#define startPIT3() PIT_TCTRL3 |= PIT_TCTRL_TEN
#define stopPIT3() PIT_TCTRL3 &= ~PIT_TCTRL_TEN

enum PIT_STATE {
  RISING_EDGE,
  FALLING_EDGE
} pit_state;

struct {
  volatile uint32_t* DATAOUTPort;
  volatile uint32_t* SCKPort;
  uint32_t DATAOUTPin;
  uint32_t SCKPin;
} pit3_pins;

volatile uint8_t newDataReady = 0;

HX711::HX711(volatile uint32_t *DATAOUTPort, uint32_t DATAOUTPin, volatile uint32_t *SCKPort,
  uint32_t SCKPin, HX711Gain gain) : _DATAOUTPort(DATAOUTPort), _SCKPort(SCKPort),
  _DATAOUTPin(DATAOUTPin), _SCKPin(SCKPin), _gain(gain) {

  // Initialize PIT module clock
  SIM_SCGC6 |= SIM_SCGC6_PIT;

  // Enable PIT module
  PIT_MCR &= ~PIT_MCR_MDIS;

  // Goal is 1 MHz communication speed with the module
  // We use module 3 as to interfere only in extreme case with Teensy
  PIT_LDVAL3 = F_BUS/1000000 - 1; // Sets to interrupt each 1 us

  // Enable Interrupts
  PIT_TFLG3 |= PIT_TFLG_TIF;
  PIT_TCTRL3 |= PIT_TCTRL_TIE;

  // Single read to set gain before first real read call
  blockingRead();
}

// TODO: Remove copying of DATAOUT Port and Pin, not useful
int32_t HX711::blockingRead() {
  // Set to currently serving HX711 pins
  pit3_pins.DATAOUTPort = _DATAOUTPort;
  pit3_pins.DATAOUTPin = _DATAOUTPin;
  pit3_pins.SCKPort = _SCKPort;
  pit3_pins.SCKPin = _SCKPin;


  uint8_t i = 0;
  int32_t value = 0;
  pit_state = FALLING_EDGE;

  // Start timer
  startPIT3();
  while (i < 24) {
    while (newDataReady == 0);
    newDataReady = 0;

    // Reads input port
    value |= ( ((*pit3_pins.DATAOUTPort) >> pit3_pins.DATAOUTPin) << i );
    i++;
  }
  while (i < _gain) {
    while (newDataReady == 0);
    newDataReady = 0;
    i++;
  }
  // Stop Timer
  stopPIT3();
  return value;
}

void pit3_isr() {
  // Clear interrupt
  PIT_TFLG3 |= PIT_TFLG_TIF;

  if(pit_state == RISING_EDGE){
    pit_state = FALLING_EDGE;
    // Toggle pin off;
    (*pit3_pins.SCKPort) &= ~(1 << pit3_pins.SCKPin);
    newDataReady = 1;
  } else {
    pit_state = RISING_EDGE;
    // Toggle pin on
    (*pit3_pins.SCKPort) |= (1 << pit3_pins.SCKPin);
  }
}
