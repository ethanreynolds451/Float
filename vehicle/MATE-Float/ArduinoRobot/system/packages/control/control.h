#ifndef CONTROL_h
#define CONTROL_h

#include "packages/include.h"

Motor motor(pin.servo);                     // Create motor object on servo pin
Limit limit(pin.limitEmpty, pin.limitFull);    // Create limit object on limit pins
Button fillButton(pin.controlFill);
Button emptyButton(pin.controlEmpty);
Button stopButton(pin.controlStop);

class Control {
  public:
    Control(){}
    bool isEmpty(){
      return limit.isEmpty();
    }
    bool isFull(){
      return limit.isFull();
    }
    void fill(){
      direction = 1;
      stopped = 0;
    }
    void empty(){
      direction = 0;
      stopped = 0;
    }
    void stop(){
      stopped = 1;
    }
    void update(){
      if(manualControl){
        checkInputs();
      }
      move();
    }
  private:
    byte speed = 255;
    bool reversed = 0;
    bool direction = 0;
    bool stopped = 1;
    bool manualControl = 0;
    void move(){
      if(stopped == 0){
        if(direction == 1){
          if(! limit.isFull()){
            motor.move((reversed != 1), speed);
            return;
          } 
        }
        if(direction == 0){
            if(! limit.isEmpty()){
              motor.move((reversed != 0), speed);
              return;
            } 
          }
      } 
      motor.stop();
    }
    void checkInputs(){
      if(stopButton.pressed()){
        stopped = 1;
      }
      if(fillButton.pressed()){
        stopped = 0;
        direction = 1;
      }
      if(emptyButton.pressed()){
        stopped = 0;
        direction = 0;
      }
    }
};

#endif