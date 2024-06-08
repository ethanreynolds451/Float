#include "src/REYAX/REYAX.h"

REYAX radio(10, 11);

char data[128] = "";


void setup(){
  Serial.begin(9600);
  radio.begin(9600);
}

void loop(){
  while(true){
    if(! radio.dataAdd("Hi")){
      break;
    }
  }
  radio.dataSend();
  delay(1000);
}
