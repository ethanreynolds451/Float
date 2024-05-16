#ifndef COMMANDS_h
#define COMMANDS_h

#include "definitions.h"

struct floatCommands {
   byte index;
   char code[9];
   bool warning;
};

class commands {
  public:
    static const byte commandLen = 10;
    const floatCommands command[commandLen] = {
      {0, "NULL", 0},
      {1, "CMEMPTY", 0},
      {2, "CMFILL", 0},
      {3, "CMSTOP", 0},
      {4, "GTTIME", 0},
      {5, "GTPRESS", 0},
      {6, "GTDATA", 0},
      {10, "CONFIRM", 0},
      {11, "GTRESP", 0},
      {12, "BRDCST", 0}
    };

    void execute(byte code){
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
      radio.send("Send OVERRIDE to execute command");
    }
};

commands c;

#endif
