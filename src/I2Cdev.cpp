#include "I2Cdev.hpp"

volatile uint8_t ack = 0;

// Include assembly symbols
void dummy_read_I2C_data(volatile uint8_t *i2c_d){ // This is dark magic
  uint32_t dummy;
  asm("LDR %[result], %[reg]" : [result] "=r" (dummy) : [reg] "r" (i2c_d));
}


//
#define startSignal() I2C0_C1 |= I2C_C1_TX; \
  I2C0_C1 |= I2C_C1_MST
#define stopSignal() I2C0_C1 &= ~I2C_C1_MST; \
  I2C0_C1 &= ~I2C_C1_TX;
#define enableAck() I2C0_C1 &= ~I2C_C1_TXAK;
#define disableAck() I2C0_C1 |= I2C_C1_TXAK;
#define writeByte(data) I2C0_D = data;
#define readByte() I2C0_D
#define repeatedStartSignal() I2C0_C1 |= I2C_C1_RSTA
#define receiveMode() I2C0_C1 &= ~I2C_C1_TX;\
  I2C0_C1 |= I2C_C1_TXAK
#define clearInterrupt() I2C0_S |= I2C_S_IICIF




void BlockingI2Cdev::waitAck() {
  while ( (I2C0_S & I2C_S_IICIF) == 0) {
    // Wait
    //Serial.print("bleh");
  }
  clearInterrupt();
}

void BlockingI2Cdev::writeRegister(uint8_t reg,uint8_t data) {
  const uint8_t addr = (this->_address << 1);
  startSignal();
  writeByte(addr);
  waitAck(); // MARK: NOT WORKING
  writeByte(data);
  waitAck();
  stopSignal();
}

uint8_t BlockingI2Cdev::readRegister(uint8_t reg) {
  startSignal();
  writeByte((_address << 1));
  waitAck();
  //Serial.println("c");
  writeByte(reg);
  waitAck();
  Serial.println("d");
  repeatedStartSignal();
  writeByte((_address << 1) | 0x1);
  waitAck();
  receiveMode();
  uint8_t data = readByte();
  stopSignal();
  return data;

}

void BlockingI2Cdev::write(uint8_t data) {
  startSignal();
  writeByte(_address << 1);
  waitAck();
  writeByte(data);
  waitAck();
  stopSignal();
}

void BlockingI2Cdev::writeRegisterBurst(uint8_t reg, uint8_t data[], uint32_t length){
  const uint8_t addr = (this->_address << 1);
  startSignal();
  writeByte(addr);
  waitAck();
  writeByte(reg);
  waitAck();
  for (uint32_t i = 0; i < length; i++) {
    writeByte(data[i]);
    waitAck();
  }
  stopSignal();
}

void BlockingI2Cdev::readRegisterBurst(uint8_t reg, uint8_t *data, uint32_t length){
  startSignal();
  writeByte(_address << 1);
  waitAck();
  writeByte(reg);
  waitAck();
  repeatedStartSignal();
  writeByte((_address << 1) | 0x1);
  waitAck();
  receiveMode();
  for (uint32_t i = 0; i < length; i++) {
    data[i] = readByte();
    if (i == (length-2)) {
      disableAck();
    }
  }
  stopSignal();
}

BlockingI2Cdev::BlockingI2Cdev(uint8_t address) : _address(address) {
}

uint8_t BlockingI2Cdev::getAddress() {
  return _address;
}
void BlockingI2Cdev::setAddress(uint8_t address) {
  _address = address;
}


void I2Cdev::initializeI2C0(uint32_t frequency) { // static
  // Enable GPIO
  SIM_SCGC5 |= SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD | SIM_SCGC5_PORTE;

  // Enable I2C0 peripheral
  SIM_SCGC4 |= SIM_SCGC4_I2C0;

  // Alt2 for PTB3 and PTB2 (G11 and G12) is I2C0
  PORTB_PCR3 |= PORT_PCR_MUX(2) | PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_DSE;;
  PORTB_PCR2 |= PORT_PCR_MUX(2) | PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_DSE;;

  // Setup Clock
  if (!(SIM_SCGC4 & SIM_SCGC4_I2C0)) return;

#if F_BUS == 120000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV1152; // 104 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV288; // 416 kHz
	} else {
		I2C0_F = I2C_F_DIV128; // 0.94 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 108000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV1024; // 105 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV256; //  422 kHz
	} else {
		I2C0_F = I2C_F_DIV112; // 0.96 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 96000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV960; // 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV240; // 400 kHz
	} else {
		I2C0_F = I2C_F_DIV96; // 1.0 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 90000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV896; // 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV224; // 402 kHz
	} else {
		I2C0_F = I2C_F_DIV88; // 1.02 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 80000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV768; // 104 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV192; // 416 kHz
	} else {
		I2C0_F = I2C_F_DIV80; // 1.0 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 72000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV640; // 112 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV192; // 375 kHz
	} else {
		I2C0_F = I2C_F_DIV72; // 1.0 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 64000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV640; // 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV160; // 400 kHz
	} else {
		I2C0_F = I2C_F_DIV64; // 1.0 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 60000000
	if (frequency < 400000) {
		I2C0_F = 0x2C;	// 104 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x1C; // 416 kHz
	} else {
		I2C0_F = 0x12; // 938 kHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 56000000
	if (frequency < 400000) {
		I2C0_F = 0x2B;	// 109 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x1C; // 389 kHz
	} else {
		I2C0_F = 0x0E; // 1 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 54000000
	if (frequency < 400000) {
		I2C0_F = I2C_F_DIV512;	// 105 kHz
	} else if (frequency < 1000000) {
		I2C0_F = I2C_F_DIV128; // 422 kHz
	} else {
		I2C0_F = I2C_F_DIV56; // 0.96 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 48000000
	if (frequency < 400000) {
		I2C0_F = 0x27;	// 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x1A; // 400 kHz
	} else {
		I2C0_F = 0x0D; // 1 MHz
	}
	I2C0_FLT = 4;
#elif F_BUS == 40000000
	if (frequency < 400000) {
		I2C0_F = 0x29;	// 104 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x19; // 416 kHz
	} else {
		I2C0_F = 0x0B; // 1 MHz
	}
	I2C0_FLT = 3;
#elif F_BUS == 36000000
	if (frequency < 400000) {
		I2C0_F = 0x28;	// 113 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x19; // 375 kHz
	} else {
		I2C0_F = 0x0A; // 1 MHz
	}
	I2C0_FLT = 3;
#elif F_BUS == 24000000
	if (frequency < 400000) {
		I2C0_F = 0x1F; // 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x12; // 375 kHz
	} else {
		I2C0_F = 0x02; // 1 MHz
	}
	I2C0_FLT = 2;
#elif F_BUS == 16000000
	if (frequency < 400000) {
		I2C0_F = 0x20; // 100 kHz
	} else if (frequency < 1000000) {
		I2C0_F = 0x07; // 400 kHz
	} else {
		I2C0_F = 0x00; // 800 MHz
	}
	I2C0_FLT = 1;
#elif F_BUS == 8000000
	if (frequency < 400000) {
		I2C0_F = 0x14; // 100 kHz
	} else {
		I2C0_F = 0x00; // 400 kHz
	}
	I2C0_FLT = 1;
#elif F_BUS == 4000000
	if (frequency < 400000) {
		I2C0_F = 0x07; // 100 kHz
	} else {
		I2C0_F = 0x00; // 200 kHz
	}
	I2C0_FLT = 1;
#elif F_BUS == 2000000
	I2C0_F = 0x00; // 100 kHz
	I2C0_FLT = 1;
#else
#error "F_BUS must be 120, 108, 96, 9, 80, 72, 64, 60, 56, 54, 48, 40, 36, 24, 16, 8, 4 or 2 MHz"
#endif

  // Enable interrupt
  I2C0_C1 |= I2C_C1_IICIE;

  // Clear pending interrupt triggered when enabling interrupt
  I2C0_S |= I2C_S_IICIF;

  // Enable I2C0
  I2C0_C2 = I2C_C2_HDRS;
	I2C0_C1 = I2C_C1_IICEN;



}



// MARK: Non blocking I2C
/*
 *
 *
 *
 *
 *
 */



// I2Cdev::I2Cdev(uint8_t address) : _address(address) {
//   // Enable interrupt
//   I2C0_C1 |= I2C_C1_IICIE;
//
//   // TODO: Add callback to callback list
//
// }

I2Cdev::I2Cdev() {
  // Enable interrupt
  I2C0_C1 |= I2C_C1_IICIE;
}

void I2Cdev::read(uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *, uint32_t)) {
  _readCompleteCallback = completeCallback;
  _bytesToRead = length;
  _rwBuffer = data;
  _currentReadIndex = 0;
  _currentState = RECEIVE;
  _currentWriteMode = ADDRESSING;
  startSignal();
  writeByte((_address << 1) | 1);
}

void I2Cdev::readRegister(uint8_t reg, uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *, uint32_t)) {
  _readCompleteCallback = completeCallback;
  _bytesToRead = length;
  _rwBuffer = data;
  _currentReadIndex = 0;
  _currentState = TRANSMIT_THEN_RECEIVE;
  _currentWriteMode = ADDRESSING;
  _currentRegister = reg;
  startSignal();
  writeByte((_address << 1) | 1);
}

void I2Cdev::write(uint8_t *data, uint32_t length, void (*completeCallback)(uint8_t *, uint32_t)) {
  _rwBuffer = data;
  _bytesToWrite = length;
  _currentWriteIndex = 0;
  _currentState = TRANSMIT;
  startSignal();
  writeByte(_address << 1);
}

void I2Cdev::callback() {
    switch (_currentState) {
    // Receiver mode
    case RECEIVE:
      if (_currentWriteMode == ADDRESSING) {
        // If last byte was a write-address, but the master is trying to read
        // from the device
        receiveMode();
        dummy_read_I2C_data(&I2C0_D);
        _currentWriteMode = DATA;
      } else {
        if (_bytesToRead == 0) {
          // If last byte, signal stop
          stopSignal();

          // Set current state to IDLE
          _currentState = IDLE;

          // Call read complete callback function
          if (*_readCompleteCallback != NULL) {
            _readCompleteCallback(_rwBuffer,_currentReadIndex+1);
            // Reset callback to NULL pointer
            _readCompleteCallback = NULL;
          }

        } else if (_bytesToRead == 1) {
          // Otherwise, if second to last, make sure last NACKs the line
          disableAck();
          // Read
          _rwBuffer[_currentReadIndex] = readByte();
          _bytesToRead--;
          _currentReadIndex++;
        } else {
          // Read
          _rwBuffer[_currentReadIndex] = readByte();
          _bytesToRead--;
          _currentReadIndex++;
        }
        break;
      }

    // Case for readRegister (repeated start)
    case TRANSMIT_THEN_RECEIVE:
      if (_currentWriteMode == ADDRESSING) {
        // If last byte was a write-address, but the master is trying to read
        // from the device
        writeByte(_currentRegister);
        _currentWriteMode = REGISTER;
      } else if(_currentWriteMode == REGISTER) {
        repeatedStartSignal();
        writeByte((_address << 1) | 1);
        _currentWriteMode = DATA;
      } else {
        _currentState = RECEIVE;
        receiveMode();
      }
      break;

    // Write mode
    case TRANSMIT:
      if (_bytesToWrite == 0) {
        // Generate stop signal
        stopSignal();

        // Call callback function
        if (_writeCompleteCallback != NULL) {
          _writeCompleteCallback(_rwBuffer, _currentWriteIndex+1);
          _writeCompleteCallback = NULL;
        }

      } else {
        I2C0_D = _rwBuffer[_currentWriteIndex];
        _currentWriteIndex++;
        _bytesToWrite--;
      }
      break;
    case IDLE:
      break;
    }
}

I2Cdev& I2Cdev::instance() {
  static I2Cdev s_instance;
  return s_instance;
}

uint8_t I2Cdev::getAddress() {
  return _address;
}
void I2Cdev::setAddress(uint8_t address) {
  _address = address;
}

// // MARK: Multiple call implementation to write/read
// struct I2CServingQueue {
//   // Index of c
//   uint8_t currentlyServing = 255;
//   uint8_t remainingToServe = 0;
//
//   // Data pointer buffer (each pointer to data from requests goes in this)
//   uint8_t* dataBuffer[NUM_I2C_CALLBACKS] = {NULL};
//
//   // Request type (selects which function to call) buffer
//   enum RequestType {WRITE, READ};
//   RequestType reqTypeBuffer[NUM_I2C_CALLBACKS];
//
//   // Callback fct pointer buffer (after operation has finished)
//   void (*i2c0_callbacks[NUM_I2C_CALLBACKS])(void) = {NULL};
//
// } i2c0ServingQueue;



// void i2c0_isr(){
//   clearInterrupt();
//   I2Cdev::instance().callback();
// }

// void i2c0_isr() {
//   if (I2C0_S & I2C_S_IICIF) {
//     ack = 1;
//   }
//   I2C0_S |= I2C_S_IICIF;
// }
