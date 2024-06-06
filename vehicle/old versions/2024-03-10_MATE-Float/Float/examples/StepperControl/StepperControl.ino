//#include <Servo.h>
#include <Tone.h>


const byte downPin = 2;
const byte stopPin = 3;
const byte upPin = 4;

const byte directionPin = 10;
const byte speedPin = 11;
const byte enablePin = 12;
const byte limitUpPin = 3;
const byte limitDownPin = 2;

Tone stepper;

void setup() {
  Serial.begin(9600);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);
  stepper.begin(speedPin);
  pinMode(directionPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
}

void loop() {
  if(digitalRead(upPin) == 0){
    move(1, 50);   
    Serial.println("Going Up");
  }
  if(digitalRead(downPin) == 0){
    move(-1, 50);
    Serial.println("Going Down");
  }
  if(digitalRead(stopPin) == 0){
    stop();
    Serial.println("Stopped");
  }
}  




void move(int dir, int spd){
  digitalWrite(enablePin, HIGH);
  if(dir == 1){
    digitalWrite(directionPin, HIGH);
  } else if (dir == -1){
    digitalWrite(directionPin, LOW);
  } else {
    return;
  }
  stepper.play(spd*50);
}

void stop(){
  stepper.stop();
  digitalWrite(enablePin, LOW);
  digitalWrite(speedPin, LOW);
  digitalWrite(directionPin, LOW);
}

void wait(int time){
  delay(time*1000);
}
