#ifndef COMMANDS_h
#define COMMANDS_h

#include "definitions.h"

// This creates a set of functions related to the execution of commands

class commands {
  private: 
    struct floatCommands {
      byte index;
      char code[8];
      bool warning;
    };
  public:
    static const byte commandLen = 15;
    const floatCommands command[commandLen] = {
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

    void stopFunctions(){
        flag.manualControl = false;          // Allow manual control until first transmission is recieved
        flag.requestTransmission = false;   // When data is recorded and waiting to transmit
        flag.broadcast = false;              // Start off broadcasting every 5 minutes
        flag.verticalProfile = false;       // Execute vertical profile functions
        flag.goToCenter = false;            // Empty float then move to center position
        flag.fillEmpty = false;             // Fill then empty ballast tank
    }

    void sendTime(){
      clear(dataString, dataLength);
      timeToString(dataString, RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());
      radio.dataAdd("UTC=");
      radio.dataAdd(dataString);
      radio.dataSend();
    }
    void sendPressure(){
      clear(dataString, dataLength);
      pressureToString(dataString, readPressure());
      radio.dataAdd("kpa=");
      radio.dataAdd(dataString);
      radio.dataSend();
    }
    
    void transmitData(){ 
      flag.requestTransmission = false;
      int dataIndex = dataCount;  // Create variable to keep track of total number of data packets
      while(dataIndex >= 0){ // Do this until all the data has been sent
        getDataString(dataIndex);  // Turn the data at specified index into a string with header
        radio.send(dataString); // Send the data over the radio
        if(radio.waitSent(2)){          // Give time to send data
            dataCount--;
        } else {                    // Otherwise, it will send the same data value again
          radio.send(dataString);   // One more chance
          if(! radio.waitSent(2)){
            failedSendAttempts++;   
          }
          dataCount--;
        }
      }
    }
    
    void sendConfirmation(){
      radio.send("Recieved");
      radio.waitSent();
    }

    bool overrideRequired(byte code){
      if (command[command[code].index].warning == 1){
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

commands c;   // Create instance of commands class

#endif
