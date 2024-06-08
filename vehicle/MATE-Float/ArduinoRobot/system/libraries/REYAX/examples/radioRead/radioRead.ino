#include <REYAX.h>

REYAX radio(10,11);

char data[128];

void setup(){
  radio.begin(9600);
  Serial.begin(9600);
}

void loop(){
  if(radio.available() > 0){
    if(radio.read(data, 128)){
      Serial.print("Recieved data: ");
      Serial.println(data);
      Serial.flush();
    }
  }
}
