#ifndef PINS_h
#define PINS_h

class Pin {
    public:
        const byte defaultTX = 2;
        const byte defaultRX = 3;
        const byte limitEmpty = 4;
        const byte limitFull = 5;
        const byte controlEmpty = 6;
        const byte controlStop = 7;
        const byte controlFill = 8;
        const byte servo = 9;
        const byte pressure = A0;
        const byte speedControl = A2;
};
Pin pin;

#endif