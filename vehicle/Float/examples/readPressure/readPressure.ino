#include <REYAX.h>

REYAX radio(2,3);

void setup() {
  radio.begin(9600);
}

void loop() {
  int pressure = map(analogRead(A0), 100, 921, 0, 100);
  char p[4];
  itoa(pressure, p, 10);
  radio.send(p);
  radio.waitSent();
  delay(1000);
}
