#ifndef INITIATE_h
#define INITIATE_h

#include "include.h"

class Float {
  public:
    Float(int TX = pin.defaultTX, int RX = pin.defaultRX){
      radio.setPins(TX, RX);
    }
    void begin(int baud = defaultBaudrate){
      system.initiatePins();
      radio.begin(baud);
      RTC.begin();
      system.run();
    }
};

#endif
