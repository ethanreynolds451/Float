#include <SoftwareSerial.h>

SoftwareSerial radio(10, 11);

void setup() {
  Serial.begin(9600);
  radio.begin(9600);
}

void loop() {
  Serial.println(status());
  delay(1000);
}

bool status(){
  bool found = false;
  radio.println("AT");
  for(int i = 0; i < 100; i++){
    if(radio.available() > 0){
      int len = radio.available();
      char buf[len + 1] = "";
      int n = 0;
      while(radio.available() > 0){
        buf[n] = radio.read();
        n++;
      }
      
      Serial.println(buf);
      if(buf[0] == '+' and buf[1] == 'O' and buf[2] == 'K'){
        found = true;
      } else {
        found = false;
      }
      break;
    }
    delay(10);
  }
  if(found == true){
    return(true);
  } else {
    return(false);
  }
}
