#ifndef SENSOR_h
#define SENSOR_h

class limit {
    public:
        int
        bool limitEmpty(){
            return ! digitalRead(pin.limitEmpty);
        }

        bool limitFull(){
            return ! digitalRead(pin.limitFull);
        }

    private:
        bool limitEmpty = 0;
        bool limitEmpty

};

class pressure {

}

#endif