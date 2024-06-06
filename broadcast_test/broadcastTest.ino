#include <REYAX.h>

REYAX radio(10, 11);

void setup() {
  Serial.begin(9600);
  radio.begin(9600);
}

void loop() {
 if(radio.status()){
    radio.send("Hello World");
    if(radio.waitSent(1)){
      Serial.println("Data Sent");
    } else {
      Serial.print("Error: ");
      Serial.println(radio.errorNumber());
    }
  } else {
    Serial.print("Error: ");
    Serial.println(radio.errorNumber());
  }
  delay(2000);
}
