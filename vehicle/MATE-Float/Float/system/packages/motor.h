#ifndef MOTOR_H
#define MOTOR_H

#include "included_libraries/Servo-1.2.1/src/Servo.h"

class motor {
    public:
        initiate(byte s){
            servo.attach(s);
        }
    private:
        Servo servo;    // The Servo object can only be directly accessed in the class
};

#endif