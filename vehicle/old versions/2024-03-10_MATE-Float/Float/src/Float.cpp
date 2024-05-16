#include "Arduino.h"
#include "Float.h"

#include "commands.h"
#include "definitions.h"

// *** Initiation and Execution Functions *** //

Float::Float(int TX, int RX){
  radio.setPins(TX, RX);
}

void Float::begin(int baud){
  Serial.begin(9600);
  radio.begin(baud);
  stepper.begin(speedPin);
  RTC.begin();
  initiatePins();
}

// Main function to read and execute commands
void Float::update(){ 
  if(broadcast&&everyFive()){
    broadcastCompanyData();
  }
  if(requestTransmission&&everyFive()){
    requestTransmitData();
  }
  if(readCommand()){
    if(! c.overrideRequired(activeCommand)){
      c.execute(activeCommand);
    }
  }
  checkLimits();
  //checkInputs();
  if(motion == 1){
    stop();
  } else if(motion == 0){
    fill();
  } else if(motion == 2){
    empty();
  }
}

// Internal Functions

bool Float::readCommand(){
  if(radio.available() > 0){
    //First, read the incomming data (automatically calls serialWait())
    radio.readData(dataIn, 64);
    //Send a confirmation message that the data was recieved and wait for it to send
    radio.send("Transmission Recieved");
    radio.waitSent();
    
    // Debug, sends length of recieved data
      //char tmp[3];
      //itoa(strlen(dataIn), tmp, 10);
      //radio.send(tmp);
      //radio.waitSent();
    
    //Decode the message and execute the command
    for(int i = 0; i < c.commandLen; i++){  // For number of potential commands
      if(strncmp(dataIn, c.command[i].code, strlen(dataIn)) == 0){  // If strings match
        activeCommand = c.command[i].index;   // Set active command to current index
        broadcast = false; // Stop broadcasting
        return true;
      }
    }
    radio.send("Command not Recognized");  
    radio.waitSent();
  }
  return false;
}

bool Float::limitEmpty(){
   return ! digitalRead(limitEmptyPin);
}

bool Float::limitFull(){
  return ! digitalRead(limitFullPin);
}


// *** Motion Control  *** //


// Empty ballast
void Float::empty(int speed){
  digitalWrite(enablePin, HIGH);
  digitalWrite(directionPin, LOW);
  stepper.play(speed*speedFactor);
}

// Fill ballast
void Float::fill(int speed){
  digitalWrite(enablePin, HIGH);
  digitalWrite(directionPin, HIGH);
  stepper.play(speed*speedFactor);
}

// Takes direction as input parameter
void Float::move(int speed, int direction){
  digitalWrite(enablePin, HIGH);
  if(direction == -1){
    digitalWrite(directionPin, HIGH);
  } else if (direction == 1){
    digitalWrite(directionPin, LOW);
  } else {
    return;
  }
  stepper.play(speed*speedFactor);
}

void Float::stop(){
  stepper.stop();
  digitalWrite(enablePin, LOW);
  digitalWrite(speedPin, LOW);
  digitalWrite(directionPin, LOW);
}

// *** Sample Data Management *** //

void Float::resetSampleData(){
  sampleDataCounter = 0;
}

void Float::getSampleData(){
  dataCount = 1;
  for(int i = 0; i < 3; i++){
    data.hour[i] = 88;
    data.minute[i] = 88;
    data.second[i] = 88;
    data.pressure[i] = sampleDataCounter + 1;
  }
  sampleDataCounter++;
}

// *** Real Data Management *** //

void Float::getData(){
  data.hour[dataCount] = RTC.getHours() + timeZoneOffset;
  data.minute[dataCount] = RTC.getMinutes();
  data.second[dataCount] = RTC.getSeconds();
  data.pressure[dataCount] = readPressure();
}

void Float::transmitData(){ 
  int totalData = dataCount;  // Create variable to keep track of total number of data packets
  while(dataCount >= 0){ // Do this until all the data has been sent
    getDataString(dataCount);  // Turn the data at specified index into a string with header
    radio.send(dataString); // Send the data over the radio
    if(radio.waitSent(2)){          // Check to make sure the data was recieved before decrementing 
      //if(radio.waitRecieved(5)){  // Make this function work in the library
        dataCount--;
      //}
    }                               // Otherwise, it will send the same data value again
  }
}

// *** RTC *** //

void Float::setTimeZone(int zone){
  timeZoneOffset = zone;
}

//bool Float::setStartCommand(char* command){
//  if(strlen(command) <= 8){
//    strcpy(startCommand, command);
//    return true;
//  }
//  return false;
//}

// *** Composite Functions *** //


void Float::verticalProfile(int speed){
  int counter = millis();   // Get time that profile started
  while(true){            // Block until down profile is complete
    if(! limitFull()){    // Fill ballast until the limit is reached
      fill(speed);             
    } else {
      stop();
    }
    if(millis() - counter >= 5000){    // Record data every 5 seconds
      counter = millis();
      getData();                    
    }
    // if(the pressure fails to change after a certain amount of time or reaches a limit){
    //   break;
    // }  
  }
  while(true){
    if(! limitEmpty()){    // Empty ballast until the limit is reached
      empty(speed);             
    } else {
      stop();
    }
    if(millis() - counter >= 5000){    // Record data every 5 seconds
      counter = millis();
      getData();                    
    }
    // if(the pressure fails to change after a certain amount of time or reaches a limit){
    //   break;
    // }     
  }
}

void Float::sendData(){
  
}

// *** Internal Functions *** //

bool Float::checkInputs(){
  if(digitalRead(controlStopPin) == 0){
    stop();
    motion = 1;
  } else if(digitalRead(controlFillPin) == 0){
    motion = 2;
  } else if(digitalRead(controlEmptyPin) == 0){
    motion = 0;
  }
}

bool Float::checkLimits(){
  // If it is going down and hits the lower limit, stop
  if((digitalRead(limitEmptyPin) == 0)&&(motion == 2)){
    stop();
    motion = 1;
    return true;
  }
  // If it is going up and hits the upper limit, stop
  if((digitalRead(limitFullPin) == 0)&&(motion == 0)){
    stop();
    motion = 1;
    return true;
  }
  return false;
}
  
void Float::getDataString(int index) {
  clear(dataString, dataLength);
  strcat(dataString, "RA01,UTC=");
  timeToString(dataString, data.hour[index], data.minute[index], data.second[index]);
// Add pressure to string
  strcat(dataString, ",PA=");
  char pressureTmp[8];
  pressureToString(pressureTmp, data.pressure[index]);
  strcat(dataString, pressureTmp);
}
 
 
