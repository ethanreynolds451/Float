#include "Float/src/Float.h"
#include "Float/src/Float.cpp"

Float cajunInjector(2, 3);

void setup(){
  cajunInjector.begin(9600);
}

void loop(){
  cajunInjector.update();
}
