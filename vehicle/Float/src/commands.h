#ifndef COMMANDS_h
#define COMMANDS_h

#include "definitions.h"

// This creates a set of functions related to the execution of commands

class commands {
  private: 
    struct floatCommands {
      byte index;
      char code[9];
      bool warning;
    };
  public:
    static const byte commandLen = 13;
    const floatCommands command[commandLen] = {
      {0, "RESET", 0},
      {1, "CMEMPTY", 0},
      {2, "CMFILL", 0},
      {3, "CMSTOP", 0},
      {4, "GTTIME", 0},
      {5, "GTPRESS", 0},
      {6, "GTDTCT", 0},
      {7, "GTDATA", 0},
      {10, "CONFIRM", 0},
      {11, "GTRESP", 0},
      {12, "BRDCST", 0},
      {13, "CMVPROF", 0},
      {20, "SMPDATA", 0}
    };

    void execute(byte code){
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
        sendTime();
      }
      if(code == 5){
        sendPressure();
      } 
      if(code == 6){
        clear(dataString, dataLength);
        itoa(dataCount, dataString, 10);
        radio.send(dataString);
      }
      if(code == 7){
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
        broadcast = true;
      } 
      if(code == 13){
        reachedBottom = false;
        verticalProfile = true;
      }
      if(code == 20){
        getSampleData();
      }
    }

    void empty(){
      motion = 2;
    }
    void fill(){
      motion = 0;
    }
    void stop(){
      motion = 1;
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
      radio.dataAdd("PA=");
      radio.dataAdd(dataString);
      radio.dataSend();
    }
    
    void transmitData(){ 
      requestTransmission = false;
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