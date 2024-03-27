#ifndef DEFINITIONS_h
#define DEFINITIONS_h

void(* resetFunc) (void) = 0;

// Includes
#include <I2C_RTC.h>
#include <Tone.h>
#include <REYAX.h>

// Object definitions
REYAX radio;                      // Create REYAX object
PCF8523 RTC;                      // Creates instance of RTC
Tone stepper;                     // Create tone instance for stepper control    

// User Defined Variables
int timeZoneOffset = +5;                                  // Time to be used for time zone correction
const int pressureRange[] = {100, 921};                   // Analog read range for pressure sensor
float profileDepth = 1;                                   // Depth of pool in m
float profileBuffer = 0.25;                               // Window in which float will change direction
const int dataLength = 27;                                // Define length of data packet
const char companyData[dataLength] = "Hello World";       // Data that will be broadcasted before vertical profile
const int dataLimit = 50;                                 /* Maximum quantity of data values to be recorded on vertical profile
                                                             All data storage space is allocated at compilation
                                                             Adjust this value to fit Arduino memory
                                                          */
// Control Variables
bool broadcast = true;              // Start off broadcasting every 5 minutes
bool requestTransmission = false;   // When data is recorded and waiting to transmit
bool verticalProfile = false;       // Execute vertical profile functions
bool reachedBottom = false;         // Start going up when float reaches bottom
bool reachedSurface = false;        // Verical profile completed when surface reached
byte motion = 1;  
byte dataSendDelay = 100;           // How many milliseconds to wait between data packet transmission

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
const byte pressurePin = A0;

// Communication and data management
char dataString[dataLength] = "";         // Create string to hold data
const int dataInLen = 64;                 // Length of buffer to hold incoming transmissions
char dataIn[dataInLen];                   // Buffer to hold incomming transmissions
struct dataArray {                        // Allocate storage for recording data
  byte hour[dataLimit];
  byte minute[dataLimit];
  byte second[dataLimit];
  int pressure[dataLimit];
};
dataArray data;                            // Create instance of dataArray named 'data'
byte failedSendAttempts = 0;

// Command processing
byte activeCommand = 0;           // Stores most recent recieved command
byte savedCommand = 0;            // Save a command for later execution, used when verification is needed

// Data Transmission  
int dataCount = 0;                // Tracks the number of data packets recorded
int sampleDataCounter = 0;        // Tracks the number of data packets recorded           

// Functions

void initiatePins() {
  pinMode(limitEmptyPin, INPUT_PULLUP);
  pinMode(limitFullPin, INPUT_PULLUP);
  pinMode(controlEmptyPin, INPUT_PULLUP);
  pinMode(controlFillPin, INPUT_PULLUP);
  pinMode(controlStopPin, INPUT_PULLUP);
  pinMode(directionPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
}

void broadcastCompanyData() {
  radio.dataAdd(companyData);
  radio.dataAdd(": Seconds = ");
  if (RTC.isRunning()) {
    char t[3];
    itoa(RTC.getSeconds(), t, 10);
    radio.dataAdd(t);
  }
  radio.dataSend();
  radio.waitSent();
}

void requestTransmitData() {
  radio.dataAdd("RQTRANSMISSION=");
  char dc[4];
  itoa(dataCount, dc, 10);
  radio.dataAdd(dc);
  radio.dataSend();
}

int readPressure() {
  /* Sensor returns linear 0.5 to 4.5 V, 0 to 100 PSI
     Maps input values from lower / upper analog read limit to scale of 1000, which will equal tenths of PSI
     Multiplies by 689.5 to convert to PA
  */
  return (689.5 * map(analogRead(pressurePin), pressureRange[0], pressureRange[1], 0, 1000)); // 1 psi = 6894.76 pa
}

int fiveSecondCounter = 0;
bool everyFive() {
  // If the RTC is working, use the second counter to time actions at 5 second intervals
  if (RTC.isRunning()) {
    int s = RTC.getSeconds();
    if (s % 5 == 0) {
      return true;
    }
  } else {
    // Adds backup method using Arduino clock if RTC failure
    if (millis() - fiveSecondCounter >= 5000) {
      fiveSecondCounter = millis();
      return true;
    }
  }
  return false;
}

void clear(char* input, int len) {
  for (int i = 0; i < len; i++) {
    input[i] = '\0';
  }
}

void timeToString(char* result, int hr, int mn, int sc) {
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

pressureToString(char* result, int pr) {
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

void getDataString(int index) {
  clear(dataString, dataLength);
  strcat(dataString, "RA01,UTC=");
  timeToString(dataString, data.hour[index], data.minute[index], data.second[index]);
  // Add pressure to string
  strcat(dataString, ",PA=");
  char pressureTmp[8];
  pressureToString(pressureTmp, data.pressure[index]);
  strcat(dataString, pressureTmp);
}

void getSampleData(){
  dataCount = 1;
  for(int i = 0; i < 3; i++){
    data.hour[i] = 88;
    data.minute[i] = 88;
    data.second[i] = 88;
    data.pressure[i] = sampleDataCounter + 1;
  }
  sampleDataCounter++;
}

#endif
