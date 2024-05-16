#ifndef SYSTEM_h
#define SYSTEM_h

#include "definitions.h"
#include "control.h"

class System {
    public:
        void start(){
            control.begin();
            while(true){
                update();
                if (exit) {
                    break;
                }
            }
        }
        void update(){
            control.step();
            if terminate
        }
        void terminate(){
            exit = true;
        }
    private:
        bool exit = false;
};
System system;

#endif
