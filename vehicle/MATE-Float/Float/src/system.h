#ifndef SYSTEM_h
#define SYSTEM_h

#include "definitions.h"

class System {
    public:
        byte motion = 1;
        int timeUnder = 0;

        unsigned long fiveSecondCounter = 0;

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

        bool everyFive() {
          // If the RTC is working, use the second counter to time actions at 5 second intervals
          if (RTC.isRunning()) {
            int s = RTC.getSeconds();
            if (s % 5 == 0) {
              return true;
            }
          } else {
            // Adds backup method using Arduino clock if RTC failure
            if (millis() - fiveSecondCounter >= 5000) {
              fiveSecondCounter = millis();
              return true;
            }
          }
          return false;
        }

};
System system;

#endif