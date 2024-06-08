#ifndef COMMAND_h
#define COMMAND_h

class Command {
  public:
    void execute(byte code){
      stopFunctions();      // Halt any other function running
      // convert code to command, NOT YET ACTIVE, NEED TO TEST
      if(code == 0){
        resetFunc();
      }
      if(code == 1){
        motion = 2;
      }
      if(code == 2){
        motion = 0;
      }
      if(code == 3){
        motion = 1; 
      }
      if(code == 4){
        flag.emptied = false;
        flag.goToCenter = true;
      }
      if(code == 5){
        sendTime();
      }
      if(code == 6){
        sendPressure();
      } 
      if(code == 7){
        clear(dataString, dataLength);
        itoa(dataCount, dataString, 10);
        radio.send(dataString);
      }
      if(code == 8){
        if(dataCount > 0){
          transmitData();
        } else {
          radio.send("No Data Available");
        }
      } 
      if(code == 10){
        execute(savedCommand);
        savedCommand = 0;
      } 
      if(code == 11){
        sendConfirmation();
      } 
      if(code == 12){
        flag.broadcast = true;
      } 
      if(code == 13){
        flag.reachedBottom = false;
        flag.verticalProfile = true;
      }
      if(code == 14){
        flag.filled = false;     // Start out assuming it is not full
        flag.emptied = false;
        flag.fillEmpty = true;    // Execute fill - empty sequence
      }
      if(code == 20){
        getSampleData();
      }
    }
  private:
    byte activeCommand = 0;           // Stores most recent recieved command
    byte savedCommand = 0;            // Save a command for later execution, used when verification is needed

    static const byte commandLen = 15;

    struct floatCommands {
      byte index;
      char code[8];
      bool warning;
    };

    const floatCommands cm[commandLen] = {
      {0, "RESET", 0},
      {1, "CMEMTY", 0},
      {2, "CMFILL", 0},
      {3, "CMSTOP", 0},
      {4, "CMCNTR", 0},
      {5, "GTTIME", 0},
      {6, "GTPRSS", 0},
      {7, "GTDTCT", 0},
      {8, "", 0},
      {10, "CONFRM", 0},
      {11, "GTRESP", 0},
      {12, "BRDCST", 0},
      {13, "CMVPRO", 0},
      {14, "CMDNUP", 0},
      {20, "GTSDTA", 0}
    };

    void stopFunctions(){
        flag.manualControl = false;          // Allow manual control until first transmission is recieved
        flag.requestTransmission = false;   // When data is recorded and waiting to transmit
        flag.broadcast = false;              // Start off broadcasting every 5 minutes
        flag.verticalProfile = false;       // Execute vertical profile functions
        flag.goToCenter = false;            // Empty float then move to center position
        flag.fillEmpty = false;             // Fill then empty ballast tank
    }

    bool overrideRequired(byte code){
      if (cm[cm[code].index].warning == 1){
        savedCommand = code;
        return true;
      }
      return false;
    }

    void confirmAction(){
      radio.send("This action could result in loss of vehicle");
      radio.send("Send CONFIRM to execute command");
    }
};





#endif
