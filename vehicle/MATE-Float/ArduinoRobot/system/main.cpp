#include "Arduino.h"
#include "Float.h"
#include "system.h"

// *** Initiation and Execution Functions *** //

Float::Float(int TX, int RX){
  radio.setPins(TX, RX);
}

void Float::begin(int baud){
  // Serial.begin(9600);
  system.initiatePins();
  radio.begin(baud);
  RTC.begin();
  system.run();
}





