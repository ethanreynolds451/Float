#include "src/REYAX.h"

REYAX radio(10, 11);

char data[128] = "";


void setup(){
  Serial.begin(9600);
  radio.begin(9600);
}

void loop(){
  radio.dataAdd("Message");
  radio.dataSend();
  if(! radio.waitSent()){
    Serial.println(radio.getErrorNumber());
  } else {
    Serial.println("Message Sent Sucessfully");
  }
  delay(1000);
}
