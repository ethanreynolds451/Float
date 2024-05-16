#ifndef CONTROL_h
#define CONTROL_h

#include "definitions.h"

#include <Servo.h>
Servo servo;

class Control {
    public:
        void begin(){
            servo.attach(pin.servo);
        }
        void step(){
            if (pos >= 180) {
                dir = -1;
            }
            if (pos <= 0) {
                dir = 1;
            }
            pos = pos + dir;
            servo.write(pos);
            delay(15);
        }
    private:
        int pos = 0;
        byte dir = 1;
};
Control control;

#endif