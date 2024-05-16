#ifndef SYSTEM_h
#define SYSTEM_h

class System {
    public:
        byte motion = 1;
        int timeUnder = 0;

        void initiatePins() {
          pinMode(limitEmptyPin, INPUT_PULLUP);
          pinMode(limitFullPin, INPUT_PULLUP);
          pinMode(controlEmptyPin, INPUT_PULLUP);
          pinMode(controlFillPin, INPUT_PULLUP);
          pinMode(controlStopPin, INPUT_PULLUP);
          servo.attach(servoPin);
        }

        class Flag {
            public:
                bool manualControl = true;          // Allow manual control until first transmission is recieved
                bool requestTransmission = false;   // When data is recorded and waiting to transmit
                bool broadcast = true;              // Start off broadcasting every 5 seconds
                bool verticalProfile = false;       // Execute vertical profile functions
                bool goToCenter = false;            // Empty float then move to center position
                bool fillEmpty = false;             // Fill then empty ballast tank
                // INDICATOR variables
                bool filled = false;
                bool emptied = false;
                bool reachedBottom = false;         // Start going up when float reaches bottom
        };
        Flag flag;      // Create instance of flag class

};

#endif