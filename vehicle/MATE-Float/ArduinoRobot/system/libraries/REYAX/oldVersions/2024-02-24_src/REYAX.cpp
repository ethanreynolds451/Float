#include "Arduino.h"
#include "REYAX.h"

//  Create instance of REYAX object with SoftwareSerial on given pins
REYAX::REYAX(int TX = 2, int RX = 3){   //  Default values for TX and RX pins
  //  This redefines the value to which the radioSerial pointer is adressed to include actual pins
  radioSerial = new SoftwareSerial(TX, RX); 
}

// Change radio output pins
void REYAX::setPins(int TX = 2, int RX = 3){
  radioSerial = new SoftwareSerial(TX, RX); 
}

//waui  Begins serial communication with radio
void REYAX::begin(int baud_rate){
  radioSerial->begin(baud_rate);
}

//  Change the maximum acceptable length of the data to send
void REYAX::setSendLength(int l){
  sendLen = l;
}

//  Change the maximum acceptable length of recieved data
void REYAX::setRecieveLength(int l){
  recLen = l;
}

void REYAX::setSendNetwork(int net = 1){
  clear(header, 12);
  strcat(header, "AT+SEND=");
  char netStr[3];
  itoa(net, netStr, 10);
  strcat(header, netStr);
  strcat(header, ",");
}

//  These three commands are to return the active error
byte REYAX::getErrorNumber(){
  return activeError;
}

char* REYAX::getErrorCode(){
    for (int i = 0; i < 16; i++) {
        if (error[i].errorNumber == activeError) {
            return error[i].errorCode;  // Return the error message if found
        }
    }
    return "No Error";  // Return "No Error" if the error is not found in the loop
}

char* REYAX::getErrorMessage() {
    for (int i = 0; i < 16; i++) {
        if (error[i].errorNumber == activeError) {
            return error[i].errorMessage;  // Return the error message if found
        }
    }
    return "No Error";  // Return "No Error" if the error is not found in the loop
}

//  Returns how many bytes are available to read
int REYAX::available(){
  return radioSerial->available();
}

void REYAX::send(char * data){    //  Accept input as pointer to data to send
  int dataLength = strlen(data);  //  Get length of input
  radioSerial->print(header);     //  Send header to radio
  radioSerial->print(dataLength); //  Send length of data to radio (needed for transmission)
  radioSerial->print(",");        //  Comma seperator for data
  radioSerial->println(data);     //  Send data with new line character
}

//  Read all incoming data into an array. Return true if there was data, false if none
bool REYAX::read(char* result, size_t targetLen){
  clear(result, targetLen);
  //  If there is data available to read
  if(radioSerial->available() > 0){
    //  Check to make sure there is enough space available to copy the data
    if(radioSerial->available() < targetLen - 1){
      int n = 0;   // Set initial char position
      //  Read each charachter into buffer
      while(radioSerial->available() > 0){  
        result[n] = radioSerial->read();
        n++;
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

//  Reads a \n terminated string into a buffer of given size
//  Returns false if no data
bool REYAX::readLine(char* result, size_t targetLen) {
  clear(result, targetLen);
  int n = 0; // Initialize character position
  char c;
  while (n < targetLen - 1) {         //  Ensure the buffer isn't exceeded
    if (radioSerial->available()) {   //  Check if there is data available
      c = radioSerial->read();        //  Read a character
      if (c == '\n') {                //  Check for newline character
        break;                        //  Exit the loop if newline is encountered
      }
      result[n++] = c;                // Store the character in the buffer and increment position
    } else {
      break;                          // If no data available, break out of the loop
      return false;
    }
  }
  result[n] = '\0'; // Null-terminate the string
}

//  Waits until a +OK message is recieved, or returns false if error
bool REYAX::waitSent(int timeout){
    /*  This method is used so it can take up to the timeout 
     *  but if it happens faster it wont be holding up the rest of the program
     */
  for(int i = 0; i < timeout*100; i++){
    if(radioSerial->available() > 0){
      read(dataIn, dataInLen);
      break;
    }
    delay(10);
  }
  return ! checkError();
}

bool REYAX::waitRecieved(int timeout){
  char response[64];
  for(int i = 0; i < timeout*100; i++){
    if(radioSerial->available() > 0){
      char response = readData();
      break;
    }
    delay(10);
  }
  return (strncmp(response, confirmation, strlen(confirmation)) == 0);
}

//  Checks the status of the radio module by sending AT command
bool REYAX::status(){
  radioSerial->println("AT");
  for(int i = 0; i < defaultTimeout*100; i++){
    if(radioSerial->available() > 0){
      read(dataIn, dataInLen);
      break;
    }
    delay(10);
  }
  return ! checkError();
}

char* REYAX::readData(){
  char data[64] = "";
  readLine(dataIn, 128);
  int startIndex = -1;
  int endIndex = -1;
  int cnt = 0;
  int length = 0;
  for(int i = 0; dataIn[i] != '\0'; i++){
    if(dataIn[i] == ','){
      cnt++;
      if(cnt == 2){
        startIndex = i + 1;
        break;
      }
    }

  }
  cnt = 0;
  for(int i = strlen(dataIn) - 1; i >= 0; i--){
    if(dataIn[i] == ','){
      cnt++;
      if(cnt == 2){
        endIndex = i - 1;
        break;
      }
    }
  }
  for (int i = 0; i < (endIndex - startIndex + 1); i++) {
        data[i] = dataIn[startIndex + i];
  }
  return data;
}

//  Get the length of the data in the send buffer
int REYAX::dataLength(){
  return strlen(dataOut);
}

//  Add to the stored data. Returns true if sucessful, false if error
bool REYAX::dataAdd(char* input){
  //  Compare the length of the data already there plus the length of the
  //  data to be added with the acceptable send length defined by setSendLen
  if((strlen(dataOut) + strlen(input)) < sendLen){
    //  If there is enough space, add to the send array and return true
    strcat(dataOut, input);
    return true;
  } else {
    //  Otherwise, return false
    return false;
  }
}

//  Clear the stored data
void REYAX::dataClear(){
  //  For each space in the dataOut array
  for (int i = 0; i < sizeof(dataOut) / sizeof(dataOut[0]); i++) {
    //  Write a null charachter
    dataOut[i] = '\0';  
  }
}

//  Return the stored data without deleting it. Returns true if sucessful, false if error. 
bool REYAX::dataGet(char* result){   //Accepts pointer to external array
  //  First, clear the array into which the result will be copied
  for (int i = 0; i < sizeof(result) / sizeof(result[0]); i++) {
    result[i] = '\0';  
  }
  //  Check to make sure there is enough space in the target array
  if((sizeof(result) / sizeof(result[0])) < (strlen(dataOut))){
    //  If so, copy the data stored in the dataOut buffer into this array and return true
    strcpy(result, dataOut);
    return true;
  } else {
    //  If not, return false to signify error
    return false;
  }
}

//  Send the stored data and delete it
void REYAX::dataSend(){
  //  Send the data then clear the dataOut array
  send(dataOut);
  dataClear();
}

//  Send the stored data without deleting it
void REYAX::dataSendKeep(){
  send(dataOut);
}


//  *************************************
//  *********** Radio Setup *************
//  *************************************

/*  bool REYAX::softwareReset(){
 *    Send: AT+RESET
 *    Response: +RESET +READY
 *  }
 *  
 *  bool REYAX::setMode(int mode){
 *    Send: AT+MODE=mode
 *    Response: +OK
 *    0:Transceiver mode (default). 1:Sleep mode.
 *  }
 *  
 *  int REYAX::getMode(){
 *    Send: AT+MODE?
 *    Reieve: +MODE=<mode>
 *  }
 *  
 *  bool REYAX::setBaudRate(int baud){
 *    Send: AT+IPR=baud
 *    Recieve: +IPR=<rate>
 *    300 1200 4800 9600 19200 28800 38400 57600 115200(default)
 *  }
 *   
 *  int REYAX::getBaudRate(){
 *    Send: AT+IPR?
 *    Recieve: +IPR=9600
 *  }
 *  
 *  bool REYAX::setFrequency(int freq){
 *    Send: AT+BAND=<Parameter>,<Frequency Memory>
 *    Recieve: +OK
 *    470000000Hz to 915000000 (default)    <M> to store parmeter
 *  }
 *  
 *  int REYAX::getFrequency(){
 *    Send: AT+BAND?
 *    Recieve: +BAND=<freq_in_hz>
 *  }
 *  
 *  void REYAX::setRFParameters(int sp, int bw, int cr, int pp){
 *    Send: AT+PARAMETER=<Spreading Factor>, <Bandwidth>,<Coding Rate>, <Programmed Preamble>
 *    Recieve: +OK
 *      <Spreading Factor>5~11 (default 9)
 *       SF7to SF9 at 125kHz, SF7 to SF10 at 250kHz, and SF7 to SF11 at 500kHz <Bandwidth>7~9, list as below:
 *       7: 125 KHz (default) 8: 250 KHz
 *       9: 500 KHz
 *       <Coding Rate>1~4, (default 1) <Programmed Preamble>(default 12)
 *       When NETWORKID=18, The value can be configured to 4~24.
 *       Other NETWORKID can only be configured to 12.
 *  }
 *  
 *  ??? REYAX::getRFparameters(){
 *  
 *  }
 *  
 *  bool REYAX::setAddress(int addr = 1){
 *    Send: AT+ADDRESS=<Address>
 *    Recieve: +OK
 *    <Address>=0~65535 (default 0)
 *  }
 *  
 *  int REYAX::getAddress(){
 *    Send: AT+ADDRESS?
 *    Recieve: +ADDRESS=120
 *  }
 *  
 *  bool REYAX::setNetworkID(int net = 1){
 *    Send: AT+NETWORKID=<Network ID>
 *    Recieve: +OK
 *    <NetworkID>=3~15,18(default18)
 *  }
 *  
 *  
 *  int REYAX::getNetworkID(){
 *    Send: AT+NETWORKID?
 *    Recieve: +NETWORK=6
 *    
 *    
 *  bool REYAX::setPassword(char* password){
 *    Send: AT+CPIN=<Password>
 *    Recieve: +OK
 *    From 00000001 to FFFFFFFF
 *    Only by using same password can the data be recognized. After resetting, the previously password will disappear.
 *  }
 *  
 *  void REYAX::getPassword(){
 *    Send: AT+CPIN?
 *    Recieve: +CPIN=No Password! or +CPIN=EEDCAA90
 *  }
 * 
 *  bool REYAX::setPower(int pwr){
 *    Send: AT+CRFOP=<power>
 *    Recieve: +OK
 *    <power>0~22 dBm
 *    22: 22dBm(default), etc.
 *    * RF Output Power must be set to less than AT+CRFOP=14 to comply CE certification.
 *  }
 *  
 *  int REYAX::getpower(){
 *    Send: AT+CRFOP?
 *    Recieve: +CRFOP=10
 *  }
 *  
 *  int REYAX::getModuleID(){
 *    Send: AT+UID?
 *    Recieve: +UID=104737333437353600170029
 *  }
 *  
 *  void REYAX::getFirmwareVersion(){
 *    Send: AT+VER?
 *    Recieve: +VER=RYLRxx8_Vx.x.x
 *  }
 *  
 *  bool REYAX::factoryReset(){
 *    Send: AT+FACTORY
 *    Recieve: +FACTORY
 *    Manufacturer defaults: BAND:915MHz UART:115200 Spreading Factor:9 Bandwidth:125kHz Coding Rate:1 Preamble Length:12 Address:0 Network ID:18 CRFOP:22
 *  }
 */
