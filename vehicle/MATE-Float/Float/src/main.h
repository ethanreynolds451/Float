#ifndef FLOAT_h
#define FLOAT_h

#include "Arduino.h"
#include "definitions.h"
#include "commands.h"

class Float {
  public:
    Float(int TX = 2, int RX = 3);
    void begin(int baud);
    void update();
};

#endif
