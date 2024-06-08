#ifndef REYAX_h
#define REYAX_h

#include "Arduino.h"
#include <SoftwareSerial.h>  
#include "ErrorCodes.h"

//  Creates reference to object "radioSerial" of type SoftwareSerial, 
//  but tells program it is defined somewhere else
extern SoftwareSerial radioSerial; 

class REYAX {
  public:
    REYAX(int TX = 2, int RX = 3);
    void setPins(int TX = 2, int RX = 3);
    void begin(int baud_rate);
    int defaultTimeout = 1;
    void setSendLength(int l);
    void setRecieveLength(int l);
    void setSendNetwork(int net = 1);  //  This is the address data will be sent to
    //  int getSendNetwork(); 
    byte getErrorNumber();
    char* getErrorCode();
    char* getErrorMessage();
    int available();
    void send(char * data);
    bool read(char* result, size_t targetLen);
    bool readLine(char* result, size_t targetLen);
    bool waitSent(int timeout);
    bool waitRecieved(int timeout);
    bool status();
    //  These functions are for returning data from the module
    char* readData();
    /* The following functions create a method for data 
     * management where multiple pieces of data can 
     * be added to a buffer and sent at the same time.
     */
    int dataLength();
    bool dataAdd(char* input);
    void dataClear();
    bool dataGet(char* result);
    void dataSend();
    void dataSendKeep();
    //  bool softwareReset();
    //  bool setMode(int mode);
    //  int getMode();
    //  bool setBaudRate(int baud);
    //  int getBaudRate();
    //  bool setFrequency(int freq);
    //  int getFrequency();
    //  bool setRFParameters(int sp, int bw, int cr, int pp);
    //  ??? getRFParameters();
    //  bool setAddress(int addr = 1);
    //  int getAddress();
    //  bool setNetworkID(int net = 1);
    //  int getNetworkID();
    //  bool setPassword(char* password);
    //  void getPassword();
    //  bool setPower(int pwr);
    //  int getPower();
    //  int getModuleID();
    //  void getFirmwareVersion();
    //  bool factoryReset();
  private:
    //  Creates pointer with name "radioSerial" of type "SoftwareSerial". 
    //  This is needed so that future dependnecies have a reference
    SoftwareSerial* radioSerial;  
    byte activeError = 0;  //  Create int to store error message
    int sendLen = 46; //  Set to fit Arduino buffer with header
    int recLen = 64; // Set to fit Arduino buffer
    char header[12] = "AT+SEND=1,";  //  Default network ID is 1
    char confirmation[8] = "RECIEVE";
    char dataIn[128] = ""; // Set aside 128 bytes for incomming data
    size_t dataInLen = (sizeof(dataIn) / sizeof(dataIn[0])); //  Gets the amount of space in this array as am int                                      //  needed to pass to functions that write into array so they don't exceed the length
    char dataOut[128] = ""; //  Set aside 128 bytes for outgoing data 
    size_t dataOutLen = (sizeof(dataOut) / sizeof(dataOut[0]));
    char password[9] = "";  //  Array to hold password if using
    
    //  Clears a charachter array by writing a null charachter to each allocated space
    void clear(char* input, int len){
      for (int i = 0; i < len; i++) {
        input[i] = '\0';
      }
    }

    void waitRead(int timeout){
      for(int i = 0; i < timeout*100; i++){
        if(radioSerial->available() > 0){
          read(dataIn, dataInLen);
          break;
        }
        delay(10);
      }
    }
    
    //  Reads the response message from the radio and checks if it is good or an error message
    bool checkError(){  //  Returns "true" if eror and "false" if none
      if((strncmp(dataIn, "+OK", 3) == false)){ //  False means they are equal
          activeError = 0;
          return false;  
      } else {
          if((strncmp(dataIn, "+ERR=", 5) == false)){
            char errStr[3];
            errStr[0] = dataIn[5];
            errStr[1] = dataIn[6];
            activeError = atoi(errStr);
          } else {
            activeError = 42;
          }
          return true;
      }
    }
};

#endif
