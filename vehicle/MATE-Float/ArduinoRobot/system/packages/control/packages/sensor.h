#ifndef SENSOR_h
#define SENSOR_h

class Limit {
    public:
        Limit(byte bottom, byte top){
            empty = bottom;
            full = top;
        }
        bool isEmpty(){
            savedEmptyState = readEmpty();
            return savedEmptyState;
        }
        bool isFull(){
            savedFullState = readFull();
            return savedFullState;
        }
    private:
        byte empty;
        byte full;
        bool readEmpty(){
            return ! digitalRead(empty);
        }
        bool readFull(){
            return ! digitalRead(full);
        }
        bool savedEmptyState = 0;
        bool savedFullState = 0;
};

class Pressure {
    public:
        Pressure(byte p, int& lower, int& upper){
            pressurePin = p;
            pressureRange[1] = lower;
            pressureRange[2] = upper;
        }
        float read() {
            /* Sensor returns linear 0.5 to 4.5 V, 0 to 100 PSI
                Maps input values from lower / upper analog read limit to scale of 1000, which will equal tenths of PSI
                Multiplies by 689.5 to convert to PA
            */
            //return map(analogRead(pressurePin), pressureRange[0], pressureRange[1], 0, 100);
            // Round it to one decimal place
            savedPressure = readPressure();
            return savedPressure;
        }
        bool increasing(){
            float reading = readPressure();
            if(reading > savedPressure){
                savedPressure = reading;
                return true;
            }
            return false;
        }
        bool decreasing(){
            float reading = readPressure();
            if(reading < savedPressure){
                savedPressure = reading;
                return true;
            }
            return false;
        }
    private:
        float readPressure(){
            return float(round(float(0.689476 * map(analogRead(pressurePin), pressureRange[0], pressureRange[1], 0, 1000))*10))/10; // 1 psi = 6894.76 pa
        }
        byte pressurePin;
        int pressureRange[2] = {100, 920};
        float savedPressure = 0.0;
};

#endif