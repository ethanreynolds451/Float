#ifndef REYAX_h
#define REYAX_h

#include "Arduino.h"
#include <SoftwareSerial.h>  
#include "ErrorCodes.h"

//  Creates reference to object "radioSerial" of type SoftwareSerial, 
//  but tells program it is defined somewhere else
extern SoftwareSerial radioSerial;
//extern errorArray error[18];  

class REYAX {
  public:
    REYAX(int TX = 2, int RX = 3);
    void setPins(int TX = 2, int RX = 3, long newBaud = -1);
    void begin(int baud = 9600);
    void setTimeout(int timeout = 1);
    void setResponseTimeout(int timeout = 3);
    void setSerialTimeout(int timeout = 250);
    void setSendLength(int l);
    void setRecieveLength(int l);
    void setSendAddress(int address = 1);  //  This is the address data will be sent to
    //bool setConfirmation(char* confirm);
    //  int getSendNetwork(); 
    byte errorNumber();
    char* errorCode();
    char* errorMessage();
    void serialWait();
    void serialWait(int timeout, int increment);
    //bool wait();  // When called, waits for functionActive to be false
    //bool wait(int waitTimeout);  
    int available();
    void send(char * data);
    bool read(char* result, size_t targetLen);
    bool readLine(char* result, size_t targetLen);
    bool waitSent();              // Overloaded functions allow unspecified value to use variable default
    bool waitSent(int timeout);
    bool waitRecieved(char* confirmation = "");
    bool waitRecieved(int timeout, char* confirmation = "");
    bool status();
    //  These functions are for returning data from the module
    bool readData(char* data, size_t len);
    /* The following functions create a method for data 
     * management where multiple pieces of data can 
     * be added to a buffer and sent at the same time.
     */
    int dataLength();
    bool dataAdd(char* input);
    void dataClear();
    char* dataGet();
    bool dataSend();
    bool dataSendKeep();
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
    void clear(char* input, int len);
  private:
    //  Creates pointer with name "radioSerial" of type "SoftwareSerial". 
    //  This is needed so that future dependnecies have a reference
    SoftwareSerial* radioSerial;  
    int savedBaudRate = 9600;
    bool serialActive = false;
    int defaultTimeout = 1;
    int defaultResponseTimeout = 3;
    int defaultSerialTimeout = 500;
    int defaultSerialIncrement = 100;
    //int defaultWaitTimeout = 10;
    byte activeError = 0;  //  Create int to store error message
    int sendLen = 46; //  Set to fit Arduino buffer with header
    int recLen = 64; // Set to fit Arduino buffer
    char header[12] = "AT+SEND=1,";  //  Default network ID is 1
    char dataIn[128] = ""; // Set aside 128 bytes for incomming data
    size_t dataInLen = (sizeof(dataIn) / sizeof(dataIn[0])); //  Gets the amount of space in this array as am int                                      //  needed to pass to functions that write into array so they don't exceed the length
    char dataOut[128] = ""; //  Set aside 128 bytes for outgoing data 
    size_t dataOutLen = (sizeof(dataOut) / sizeof(dataOut[0]));
    char password[9] = "";  //  Array to hold password if using
    bool functionActive = false;   // Creates variable to store condition
    void waitRead(int timeout);
    bool checkError();
};

#endif
