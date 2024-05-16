#include "Arduino.h"
#include "Float.h"

#include "commands.h"
#include "definitions.h"

// *** Initiation and Execution Functions *** //

Float::Float(int TX, int RX){
  radio.setPins(TX, RX);
}

void Float::begin(int baud){
  //Serial.begin(9600);
  initiatePins();
  radio.begin(baud);
  // stepper.begin(speedPin);
  RTC.begin();

}

// Main function to read and execute commxands
void Float::update(){
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





