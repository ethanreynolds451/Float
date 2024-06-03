#ifndef SYSTEM_h
#define SYSTEM_h

#include "definitions.h"

class System {
    public:
        byte motion = 1;
        int timeUnder = 0;

        void run() {
          // adjustSpeed();
  checkLimits();


  if(everyFive()){
    //Serial.println(speed);
    // Start off by broadcasting company data every five seconds
    if(flag.broadcast){
      broadcastCompanyData();
    }
    // When doing vertical profile, capture data every five seconds
    if(flag.verticalProfile){
      //recordData();
      if(readPressure() >= float(kpa_to_m*(float(profileDepth - profileBuffer)))){
        flag.reachedBottom = true;
        //radio.send("Reached Bottom; Going Up");
      }
      // If it has reached the surface, stop recording data and request transmission
      if(flag.reachedBottom&&(readPressure() <= float(kpa_to_m*profileBuffer))){
        flag.verticalProfile = false;
        //radio.send("Reached Surface");
        flag.requestTransmission = true;
      }
    }
    // When vertical profile is completed, send data transmission request every five seconds
    if(flag.requestTransmission){
      requestTransmitData();
    }
    //  if(readPressure() >= kpa_to_m*profileBuffer){
    //      timeUnder = timeUnder + 5;
    //      radio.send("Below Surface");
    //  }
  }
  if(readCommand()){
    if(! c.overrideRequired(activeCommand)){
      c.stopFunctions();
      c.execute(activeCommand);
    } else {
      c.confirmAction();
    }
  }
  // Vertical profile operations
  if(flag.verticalProfile){
    // While decending, fill ballast until it is full
    if((! flag.reachedBottom)&&(! limitFull())){
      motion = 0;
    // While ascending, empty ballast until empty
    } else if ((flag.reachedBottom)&&(! limitEmpty())){
      motion = 2;
    // Otherwise, remain stopped
    } else {
      motion = 1;
    }
  }
  if(flag.fillEmpty){
    if(! flag.filled){           // If it has not yet filled
        motion = 0;         // Fill until limit is reached
    } else if(! flag.emptied) {  // Once it has filled
        motion = 2;         // Empty until the limit is reached
    } else {                // Once it has filled and emptied
        flag.fillEmpty = false;  // Break sequence
        radio.send("Completed Up / Down Sequence");
        radio.waitSent();
    }
  }
  if(flag.goToCenter){
    if(! flag.emptied){      // If it has not yet emptied
        motion = 2;     // Empty until limit is reached
    } else {            // Once it has emptied
        flag.goToCenter = false; // Break sequence
    }
  }
  if(flag.manualControl){
    checkInputs();      // This is for manual hardware control
  }
  //    if(timeUnder > maxTimeUnder){
  //      radio.send("Resurface Failsafe Activated"); //For debug
  //      timeUnder = 0;
  //      c.stopFunctions();
  //      motion = 2;
  //    }
  if(motion == 1){
    stop();
  } else if(motion == 0){
    fill();
  } else if(motion == 2){
    empty();
  }

        }

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