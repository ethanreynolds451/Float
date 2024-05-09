#include "Arduino.h"
#include "Float.h"

#include "commands.h"
#include "definitions.h"

// *** Initiation and Execution Functions *** //

Float::Float(int TX, int RX){
  radio.setPins(TX, RX);
}

void Float::begin(int baud){
  // Serial.begin(9600);
  initiatePins();
  radio.begin(baud);
  // stepper.begin(speedPin);
  RTC.begin();

}

// Main function to read and execute commxands
void Float::update(){
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
      recordData();
      if(readPressure() >= float(kpa_to_m*(float(profileDepth - profileBuffer)))){
        flag.reachedBottom = true;
      }
      // If it has reached the surface, stop recording data and request transmission
      if(flag.reachedBottom&&(readPressure() <= float(kpa_to_m*profileBuffer))){
        flag.verticalProfile = false;
        flag.requestTransmission = true;
      }
    }
    // When vertical profile is completed, send data transmission request every five seconds
    if(flag.requestTransmission){
      requestTransmitData();
    }
    if(readPressure() >= profileBuffer){
        timeUnder += 5;
    }
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
  if(timeUnder > maxTimeUnder){
    timeUnder = 0;
    c.stopFunctions();
    motion = 2;
  }
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

bool Float::limitEmpty(){
   return ! digitalRead(limitEmptyPin);
}

bool Float::limitFull(){
  return ! digitalRead(limitFullPin);
}


// *** Motion Control  *** //

// Empty ballast
void Float::empty(){
  // digitalWrite(enablePin, HIGH);
  // digitalWrite(directionPin, LOW);
  // stepper.play(speed);
  servo.writeMicroseconds(2500);
}

// Fill ballast
void Float::fill(){
  // digitalWrite(enablePin, HIGH);
  // digitalWrite(directionPin, HIGH);
  // stepper.play(speed);
  servo.writeMicroseconds(500);
}

// Takes direction as input parameter
void Float::move(int direction){
  // digitalWrite(enablePin, HIGH);
  if(direction == -1){
    empty();
    // digitalWrite(directionPin, HIGH);
  } else if (direction == 1){
    fill();
    // digitalWrite(directionPin, LOW);
  } else {
    return;
  }
  // stepper.play(speed);
}

void Float::stop(){
  // stepper.stop();
  // digitalWrite(enablePin, LOW);
  // digitalWrite(speedPin, LOW);
  // digitalWrite(directionPin, LOW);
  servo.writeMicroseconds(1500);
}

// *** Sample Data Management *** //

void Float::resetSampleData(){
  sampleDataCounter = 0;
}


// *** Real Data Management *** //

void Float::recordData(){
  if(RTC.isRunning()){
    data.hour[dataCount] = static_cast<uint8_t>(RTC.getHours() + timeZoneOffset);
    data.minute[dataCount] = static_cast<uint8_t>(RTC.getMinutes());
    data.second[dataCount] = static_cast<uint8_t>(RTC.getSeconds());
  } else {
    data.hour[dataCount] = 62;
    data.minute[dataCount] = 62;
    data.second[dataCount] = 62;
  }
  float pressure = readPressure();
  data.pressure_int[dataCount] = int(pressure);
  data.pressure_decimal[dataCount] = static_cast<int>((pressure - static_cast<int>(pressure)) * 10);
  data.recieved[dataCount] = 0;
  dataCount++;
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


void Float::sendData(char* values){

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
    flag.emptied = true;
    motion = 1;
    return true;
  }
  // If it is going up and hits the upper limit, stop
  if((digitalRead(limitFullPin) == 0)&&(motion == 0)){
    stop();
    flag.filled = true;
    motion = 1;
    return true;
  }
  return false;
}

// void Float::adjustSpeed(){
//     if (analogRead(speedControlPin) > 10){
//         speed = map(analogRead(speedControlPin), 0, 1023, 0, 500);
//     } else {
//         speed = default_speed;
//     }
// }
 
 
