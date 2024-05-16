#ifndef CONTROL_h
#define CONTROL_h

#include "definitions.h"

// Empty ballast
void Float::empty(){
  // digitalWrite(enablePin, HIGH);
  // digitalWrite(directionPin, LOW);
  // stepper.play(speed);
  servo.writeMicroseconds(2500);
}

// Fill ballast
void Float::fill(){
  // digitalWrite(enablePin, HIGH);
  // digitalWrite(directionPin, HIGH);
  // stepper.play(speed);
  servo.writeMicroseconds(500);
}

// Takes direction as input parameter
void Float::move(int direction){
  // digitalWrite(enablePin, HIGH);
  if(direction == -1){
    empty();
    // digitalWrite(directionPin, HIGH);
  } else if (direction == 1){
    fill();
    // digitalWrite(directionPin, LOW);
  } else {
    return;
  }
  // stepper.play(speed);
}

void Float::stop(){
  // stepper.stop();
  // digitalWrite(enablePin, LOW);
  // digitalWrite(speedPin, LOW);
  // digitalWrite(directionPin, LOW);
  servo.writeMicroseconds(1500);
}

bool Float::checkInputs(){
  if(digitalRead(controlStopPin) == 0){
    stop();
    motion = 1;
  } else if(digitalRead(controlFillPin) == 0){
    motion = 2;
  } else if(digitalRead(controlEmptyPin) == 0){
    motion = 0;
  }
}

bool Float::checkLimits(){
  // If it is going down and hits the lower limit, stop
  if((digitalRead(limitEmptyPin) == 0)&&(motion == 2)){
    stop();
    flag.emptied = true;
    motion = 1;
    return true;
  }
  // If it is going up and hits the upper limit, stop
  if((digitalRead(limitFullPin) == 0)&&(motion == 0)){
    stop();
    flag.filled = true;
    motion = 1;
    return true;
  }
  return false;
}

#endif