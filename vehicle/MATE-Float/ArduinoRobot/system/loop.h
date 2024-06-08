#ifndef LOOP_h
#define LOOP_h

class Loop {
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

readCommand(){
  if(radio.available() > 0){
    //First, read the incomming data (automatically calls serialWait())
    radio.readData(dataIn, 64);
    // Debug, sends length of recieved data
      //char tmp[3];
      //itoa(strlen(dataIn), tmp, 10);
      //radio.send(tmp);
      //radio.waitSent();

    //Decode the message and execute the command
    for(int i = 0; i < c.commandLen; i++){  // For number of potential commands
      if(strncmp(dataIn, c.command[i].code, strlen(dataIn)) == 0){  // If strings match
        activeCommand = c.command[i].index;   // Set active command to current index
        flag.broadcast = false; // Stop broadcasting
        flag.manualControl = false; // Disable manual control
      // char tmp[3];
      // itoa(activeCommand, tmp, 10);
      // radio.send(tmp);
      // radio.waitSent();
        //Send a confirmation message that the data was recieved and wait for it to send
        radio.dataAdd("Executing ");
        radio.dataAdd(dataIn);
        radio.dataSend();
        radio.waitSent();
        return true;
      }
    }
    radio.send("Command not Recognized");
    radio.waitSent();
  }
  return false;
}

#endif