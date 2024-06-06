#include "src/Float.h"

Float cajunInjector(2, 3);

void setup() {
  Serial.begin(9600);
  cajunInjector.begin(9600);
}

void loop() {
  cajunInjector.getSampleData();
  cajunInjector.transmitData();
  Serial.println("Data Sent Sucessfully");
}
