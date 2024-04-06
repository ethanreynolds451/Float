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
  private:
    bool readCommand();
    bool checkLimits();
    bool limitEmpty();
    bool limitFull();
    void empty();
    void fill();
    void move(int direction);
    void stop();
    void resetSampleData();
    void recordData();
    void setTimeZone(int zone);
    bool setStartCommand(char* command);
    void sendData(char* values);
    bool checkInputs();
    void adjustSpeed();
};

#endif
