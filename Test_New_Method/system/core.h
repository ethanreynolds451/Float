#ifndef CORE_h
#define CORE_h

#include "definitions.h"
#include "packages/include.h"

class Core {
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
        }
        void terminate(){
            exit = true;
        }
    private:
        bool exit = false;
};

#endif
