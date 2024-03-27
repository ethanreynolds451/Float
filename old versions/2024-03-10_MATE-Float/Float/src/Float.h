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
    dataArray data;
    bool readCommand();
    bool limitEmpty();
    bool limitFull();
    void empty(int speed = 40);
    void fill(int speed = 40);
    void move(int direction, int speed);
    void stop();
    void resetSampleData();
    void getSampleData();
    void getData();
    void transmitData();
    void setTimeZone(int zone);
    bool setStartCommand(char* command);
    void verticalProfile(int speed);
    void sendData();
    bool checkLimits();
    bool checkInputs();
    void clear(char* input, int len);
    void getDataString(int index);
    
};

#endif
