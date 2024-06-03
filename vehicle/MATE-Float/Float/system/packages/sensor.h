#ifndef SENSOR_h
#define SENSOR_h

class limit {
    public:
        bool isLimitEmpty(){
            emptyState = readEmpty;
            return emptyState;
        }
        bool isLimitFull(){
            fullState = readFull;
            return fullState;
        }
    private:
        bool readEmpty(){
            return ! digitalRead(pin.limitEmpty);
        }
        bool readFull(){
            return ! digitalRead(pin.limitFull);
        }
        bool emptyState = 0;
        bool fullState = 0;

};

class pressure {

};

#endif