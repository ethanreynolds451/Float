#include "Arduino.h"
#include "REYAX.h"

/* ================================     
 *     Library Setup Functions
 * ================================  
 */ 

//  Create instance of REYAX object with SoftwareSerial on given pins
REYAX::REYAX(int TX, int RX){   //  Default values for TX and RX pins
  //  This redefines the value to which the radioSerial pointer is adressed to include actual pins
  radioSerial = new SoftwareSerial(TX, RX); 
}

//  Begins serial communication with radio
void REYAX::begin(int baud){
  radioSerial->begin(baud);
  savedBaudRate = baud;
  serialActive = true;
}

// Change radio output pins
void REYAX::setPins(int TX, int RX, long newBaud = -1){
  if (serialActive) {
    radioSerial->end(); // Close the serial connection if active
  }
  delete radioSerial;
  radioSerial = new SoftwareSerial(TX, RX);
  if(serialActive){
    if(newBaud != -1){
      savedBaudRate = newBaud;
    }
    radioSerial->begin(savedBaudRate);
  }
}

// Changes the default timeout for radio communication functions. 
void REYAX::setTimeout(int timeout){
  defaultTimeout = timeout;
}

// Changes the default timeout for functions that wait for a response from a receiving radio.
void REYAX::setResponseTimeout(int timeout){
  defaultResponseTimeout = timeout;
}

// Changes the default timeout for serial reading. 
void REYAX::setSerialTimeout(int timeout){
  defaultSerialTimeout = timeout;
}

//// Changes the default timeout for blocking functions 
//void REYAX::setWaitTimeout(int timeout){
//  defaultWaitTimeout = timeout;
//}

//  Change the maximum acceptable length of the data to send
void REYAX::setSendLength(int l){
  sendLen = l;
}

//  Change the maximum acceptable length of recieved data
void REYAX::setRecieveLength(int l){
  recLen = l;
}

//bool REYAX::setConfirmation(char* confirm){
//  if(strlen(confirm) <= 8){
//    strcpy(confirmation, confirm);
//    return true;
//  } else {
//    return false;
//  }
//}

void REYAX::setSendAddress(int address){
  clear(header, 12);
  strcat(header, "AT+SEND=");
  char addStr[3];
  itoa(address, addStr, 10);
  strcat(header, addStr);
  strcat(header, ",");
}

/* ================================     
 * General Communication Functions
 * ================================  
 */ 

void REYAX::serialWait(){
  serialWait(defaultSerialTimeout, defaultSerialIncrement);
}

void REYAX::serialWait(int timeout, int increment){
  unsigned long startTime = millis(); // Set initial time
  while((millis() - startTime) < timeout){   //Exit the loop if timeout exceeded
    int initialSerial = radioSerial->available();
    delay(increment);
    if(radioSerial->available() > initialSerial){
      timeout = 0;
    }
  }
}


// Wait until another function has finished completing, if applicable (blocking read)
//bool REYAX::wait(){
//  wait(defaultWaitTimeout);
//}
//
//bool REYAX::wait(int timeout){
//  unsigned long startTime = millis();
//  while(functionActive == true){
//    if(millis() - startTime >= (timeout*1000)){ // Check if timeout has elapsed   
//      return false;  // Timeout reached, return false
//    }
//    delay(1);
//  }
//  return true;
//}

//  These three commands are to return the active error
byte REYAX::errorNumber(){
  return activeError;
}

char* REYAX::errorCode(){
    for (int i = 0; i < (sizeof(error)/sizeof(error[0])); i++) {
        if (error[i].errorNumber == activeError) {
            return error[i].errorCode;  // Return the error message if found
        }
    }
    return "No Error";  // Return "No Error" if the error is not found in the loop
}

char* REYAX::errorMessage() {
    static char defaultErrorMessage[] = "No Error";
    for (int i = 0; i < (sizeof(error)/sizeof(error[0])); i++) {
        if (error[i].errorNumber == activeError) {
            return error[i].errorMessage;char* message = new char[strlen(error[i].errorMessage) + 1];
            strcpy(message, error[i].errorMessage);
            return message;                             // Return the error message if found
        }
    }
    return defaultErrorMessage;  // Return "No Error" if the error is not found in the loop
}

//  Returns how many bytes are available to read
int REYAX::available(){
  return radioSerial->available();
}

void REYAX::send(char * data){    //  Accept input as pointer to data to send
  int dataLength = strlen(data);  //  Get length of input
  if(dataLength > sendLen){
    dataLength = sendLen;
    data[dataLength] = '\0';
  }
  radioSerial->print(header);     //  Send header to radio
  radioSerial->print(dataLength); //  Send length of data to radio (needed for transmission)
  radioSerial->print(",");        //  Comma seperator for data
  radioSerial->println(data);     //  Send data with new line character
  radioSerial->flush();
}

//  Read all incoming data into an array. Return true if there was data, false if none
bool REYAX::read(char* result, size_t targetLen){
  serialWait();
  clear(result, targetLen);
  int n = 0;   // Set initial char position
  if(radioSerial->available() > 0){
    //  Check to make sure there is enough space available to copy the data
    while((radioSerial->available() > 0)&&(n < (targetLen - 1))){
      result[n] = radioSerial->read();
       n++;
    }
    if(n < (targetLen - 1)){
      activeError = 30;
      return true;
    } else {
      activeError = 0;
      return false;
    }
    
  } else {  // If there is no data on function call
    activeError = 31; // Return "no data" error
    return false; // Exit and return false
  }
}

//  Reads a \n terminated string into a buffer of given size. Returns false if no data
bool REYAX::readLine(char* result, size_t targetLen) {
  serialWait();
  clear(result, targetLen); // Clear any existing data from the buffer
  int n = 0; // Initialize character position
  if(radioSerial->available() > 0){ // Check if there is any data to be read
    char c; // Create char variable to hold incomming charachter
    while ((n < targetLen - 1)&&(radioSerial->available() > 0)) {  // Stop if buffer exceeded or all data read
      c = radioSerial->read();        //  Read a character       
      if (c == '\n') {                //  Check for newline character
        break;                        //  Exit the loop if newline is encountered
      }
      result[n++] = c;                // Store the character in the buffer and increment position
    }
  } else {                            // If no data available, break out of the loop
    activeError = 31;
    return false;                         
  }
  result[n] = '\0'; // Null-terminate the string
  activeError = 0;
  //  Clear the rest of the serial buffer
  while (radioSerial->available() > 0) {
    radioSerial->read(); 
  }
  return true;
}

// Parse data from the rest of the transmission
bool REYAX::readData(char* data, size_t len) {
    clear(data, len);
    if (!readLine(dataIn, dataInLen)) {
        return false;
    }
    int startIndex = -1;
    int endIndex = -1;
    int cnt = 0;
    // Find the start index of the second comma-separated value
    for (int i = 0; dataIn[i] != '\0'; i++) {
        if (dataIn[i] == ',') {
            cnt++;
            if (cnt == 2) {
                startIndex = i + 1;
                break;
            }
        }
    }
    cnt = 0;
    // Find the end index of the second comma-separated value
    for (int i = strlen(dataIn) - 1; i >= 0; i--) {
        if (dataIn[i] == ',') {
            cnt++;
            if (cnt == 2) {
                endIndex = i - 1;
                break;
            }
        }
    }
    // Ensure endIndex is within bounds
    endIndex = min(endIndex, (int)(strlen(dataIn) - 1));
    if ((endIndex - startIndex + 1) > len) {  // If there isn't enough space in the buffer
        activeError = 30;  // Set error to return not enough buffer space
        endIndex = startIndex + len - 1; // Truncate the data by adjusting the end index to fit the buffer
    }
    // Copy the data between startIndex and endIndex into the provided array
    for (int i = startIndex; i <= endIndex && i - startIndex < len - 1; i++) {
        data[i - startIndex] = dataIn[i];
    }
    // Ensure null-termination of the string
    data[min(endIndex - startIndex + 1, (int)len - 1)] = '\0';
    return activeError == 0;
}

//  Waits until a +OK message is recieved, or returns false if error
bool REYAX::waitSent(){
  return waitSent(defaultTimeout);
}

bool REYAX::waitSent(int timeout){
    /*  This method is used so it can take up to the timeout 
     *  but if it happens faster it wont be holding up the rest of the program
     */
  clear(dataIn, dataInLen);
  for(int i = 0; i < timeout*100; i++){
    if(radioSerial->available() > 0){
      read(dataIn, dataInLen);
      break;
    }
    delay(10);
  }
  return ! checkError();
}

bool REYAX::waitRecieved(char* confirmation){
  return waitRecieved(defaultResponseTimeout, confirmation);
}

bool REYAX::waitRecieved(int timeout, char* confirmation){
 if(! waitSent()){
    return false;
  }
  for(int i = 0; i < timeout*100; i++){
    if(radioSerial->available() > 0){
      readData(dataIn, dataInLen);
      break;
    }
    delay(10);
  }
  // Check and see if the confirmation string is defined
  if(strlen(confirmation) > 0){
    // If so, verify that the recieved data matches
    if(strncmp(dataIn, confirmation, strlen(confirmation)) == 0){
      activeError = 0;  // If so, return true: recieved data matches defined confirmation
      return true;
    } else {
      activeError = 41; // Otherwise, retuirn false: strings do not match
      return false;
    }
  // Otherwise, see if any data is recieved
  } else if(strlen(dataIn) > 0){
    return true;  // If so, return true: a response was recieved
  } else {
    activeError = 41;
    return false; // Otherwise, return false: no data was recieved
  }
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

/* ================================     
 *    Data Send Buffer Functions
 * ================================  
 */ 

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
    activeError = 0;
    return true;
  } else {
    //  Otherwise, return false
    activeError = 30;  // Data too long error
    return false;
  }
}

//  Clear the stored data
void REYAX::dataClear(){
  //  For each space in the dataOut array
  for (int i = 0; i < dataOutLen; i++) {
    //  Write a null charachter
    dataOut[i] = '\0';  
  }
}

//  Return the stored data without deleting it. Returns true if sucessful, false if error. 
char* REYAX::dataGet(){   //Accepts pointer to external array
  char dataReturn[strlen(dataOut)+1];  // Create char array to return
  strcpy(dataReturn, dataOut);
  return dataReturn;
}

//  Send the stored data and delete it
bool REYAX::dataSend(){
  //  Send the data then clear the dataOut array
  send(dataOut);
  dataClear();
  return waitSent();
}

//  Send the stored data without deleting it
bool REYAX::dataSendKeep(){
  send(dataOut);
  return waitSent();
}


/* ================================     
 *       Radio Setup Functions
 * ================================  
 */ 

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

  
//  Clears a charachter array by writing a null charachter to each allocated space
void REYAX::clear(char* input, int len){
  for (int i = 0; i < len; i++) {
    input[i] = '\0';
  }
}
 
 // Private Functions

    void REYAX::waitRead(int timeout){
      for(int i = 0; i < timeout*100; i++){
        if(radioSerial->available() > 0){
          read(dataIn, dataInLen);
          break;
        }
        delay(10);
      }
    }
    
    //  Reads the response message from the radio and checks if it is good or an error message
    bool REYAX::checkError(){  //  Returns "true" if eror and "false" if none
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
            activeError = 40;
          }
         return true;
      }
    }
