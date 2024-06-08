#ifndef INITIATE_h
#define INITIATE_h

#include "include.h"
#include "loop.h"

REYAX radio;                      // Create REYAX object
PCF8523 RTC;                      // Create instance of RTC
Servo servo;                      // Create instance of Servo

Control control;
Loop loop;

class Float {
  public:
    Float(int TX = pin.defaultTX, int RX = pin.defaultRX){
      radio.setPins(TX, RX);
    }
    void begin(int baud = defaultBaudrate){
      initiatePins();
      initiateObjects();
      loop.run();
    }
  private:
    void initiateObjects(){
      radio.begin(baud);
      RTC.begin();
    }
};

#endif
