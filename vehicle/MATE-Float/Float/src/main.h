#ifndef FLOAT_h
#define FLOAT_h

#include "Arduino.h"
#include "libraries.h"

class Float {
  public:
    Float(int TX = 2, int RX = 3);
    void begin(int baud);
    void update();
};

#endif
