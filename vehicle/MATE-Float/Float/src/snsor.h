#ifndef SENSOR_h
#define SENSOR_h

#include "definitions.h"

bool Float::limitEmpty(){
   return ! digitalRead(limitEmptyPin);
}

bool Float::limitFull(){
  return ! digitalRead(limitFullPin);
}

#endif