#ifndef DEFINITIONS_h
#define DEFINITIONS_h

// Includes
#include <I2C_RTC.h>
#include <Tone.h>
#include <REYAX.h>

// Pin Definitions
const byte defaultTX = 2;
const byte defaultRX = 3;
const byte limitEmptyPin = 4;
const byte limitFullPin = 5;
const byte controlEmptyPin = 6;
const byte controlStopPin = 7;
const byte controlFillPin = 8;
const byte directionPin = 10;
const byte speedPin = 11;
const byte enablePin = 12;
const int speedFactor = 50;

// Communication and data management
REYAX radio; 
const int dataLength = 27;
char dataString[dataLength] = "";
const int dataLimit = 50;
const char companyData[dataLength] = "Hello World";
const int dataInLen = 64;
char dataIn[dataInLen];
struct dataArray {
  byte hour[dataLimit];
  byte minute[dataLimit]; 
  byte second[dataLimit]; 
  int pressure[dataLimit];
};

// Command processing
bool broadcast = true;
byte activeCommand = 0;
byte savedCommand = 0;

// Data Transmission
bool requestTransmission = false;
int dataCount = 0;
int sampleDataCounter = 0;

// Time management
PCF8523 RTC;
int fiveSecondCounter = 0;

// RTC
int timeZoneOffset = +5;

// Pressure sensor
const int pressureRange[] = {100, 921};

// Stepper
Tone stepper;
byte motion = 1;

// Functions
void initiatePins(){
  pinMode(limitEmptyPin, INPUT_PULLUP);
  pinMode(limitFullPin, INPUT_PULLUP);
  pinMode(controlEmptyPin, INPUT_PULLUP);
  pinMode(controlFillPin, INPUT_PULLUP);
  pinMode(controlStopPin, INPUT_PULLUP);
}

void broadcastCompanyData(){
  radio.dataAdd(companyData);
  radio.dataAdd(": Seconds = ");
  if(RTC.isRunning()){
    char t[3];
    itoa(RTC.getSeconds(), t, 10);
    radio.dataAdd(t);
  }
  radio.dataSend();
  radio.waitSent();
}

void requestTransmitData(){
  radio.dataAdd("RQTRANSMISSION=");
  char dc[4];
  itoa(dataCount,dc, 10);
  radio.dataAdd(dc);
  radio.dataSend();
}

int readPressure(){
  /* Sensor returns linear 0.5 to 4.5 V, 0 to 100 PSI
   * Maps input values from lower / upper analog read limit to scale of 1000, which will equal tenths of PSI
   * Multiplies by 689.5 to convert to PA
   */
   return (689.5*map(analogRead(A0), pressureRange[0], pressureRange[1], 0, 1000));   // 1 psi = 6894.76 pa
}

bool everyFive(){
  // If the RTC is working, use the second counter to time actions at 5 second intervals
  if(RTC.isRunning()){
    int s = RTC.getSeconds();
    if (s%5 == 0){
      return true;
    }
  } else {
    // Adds backup method using Arduino clock if RTC failure
    if(millis() - fiveSecondCounter >= 5000){
      fiveSecondCounter = millis();
      return true;
    }
  }
  return false;
}

void clear(char* input, int len){
  for (int i = 0; i < len; i++) {
    input[i] = '\0';
  }
}

void timeToString(char* result, int hr, int mn, int sc){
  // Add hour to string
  if (hr < 10) {
      strcat(result, "0");
  }
  char tmp[7];
  itoa(hr, tmp, 10);
  strcat(result, tmp);
  strcat(result, ":");
  // Add minute to string
  if (mn < 10) {
      strcat(result, "0");
  }
  itoa(mn, tmp, 10);
  strcat(result, tmp);
  strcat(result, ":");
// Add second to string
  if (sc < 10) {
      strcat(result, "0");
  }
  itoa(sc, tmp, 10);
  strcat(result, tmp);
  strcat(result, ":");
}

pressureToString(char* result, int pr){
  if (pr < 10) {
      strcat(result, "0000");
  } else if (pr < 100) {
      strcat(result, "000");
  } else if (pr < 1000) {
      strcat(result, "00");
  } else if (pr < 10000) {
      strcat(result, "0");
  }
  char pCh[6];
  itoa(pr, pCh, 10);
  strcat(result, pCh);
}

#endif
