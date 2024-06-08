#include "ArduinoRobot.h"

void setup(){
    Serial.begin(9600);
}

void loop(){
  if (control.isEmpty()){
    Serial.println("Empty");
  }
  if (control.isFull()){
    Serial.println("Full");
  }
}
