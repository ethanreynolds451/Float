#ifndef FLOAT_h
#define FLOAT_h

#include "Arduino.h"
#include "system.h"

class Float {
  public:
    Float(int TX = pin.defaultTX, int RX = pin.defaultRX);
    void begin(int baud = defaultBaudrate);
};

#endif
