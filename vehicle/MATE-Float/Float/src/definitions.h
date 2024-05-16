#ifndef DEFINITIONS_h
#define DEFINITIONS_h

void(* resetFunc) (void) = 0;     // Define system reset function

// Includes
#include <I2C_RTC.h>
#include <REYAX.h>
#include <Servo.h>

// Object definitions
REYAX radio;                      // Create REYAX object
PCF8523 RTC;                      // Create instance of RTC
Servo servo;                      // Create instance of Servo

// *** User-Defined Variables ***

int timeZoneOffset = +5;                                 // Time to be used for time zone correction
const int maxTimeUnder = 180;                            // Maximum time float will remain below surface before acivating failsafe
const int pressureRange[2] = {98, 921};                  // Calibrate analog read range for pressure sensor
const float profileDepth = 3;                            // Depth of profile in meters
const float profileBuffer = 0.5;                         // Window in which float will change direction
const int dataLength = 64;                               // Define length of data packet
const char companyData[dataLength] = "RA06";             // Data that will be broadcasted before vertical profile
const int dataLimit = 30;                                 /* Maximum quantity of data values to be recorded on vertical profile
                                                             All data storage space is allocated at compilation
                                                             Adjust this value to fit Arduino memory
                                                          */

const float kpa_to_m = 9.792;                            // Convert kpa to meters fresh water
byte dataSendDelay = 100;           // How many milliseconds to wait between data packet transmission



// *** Pin Definitions ***

const byte defaultTX = 2;
const byte defaultRX = 3;
const byte limitEmptyPin = 4;
const byte limitFullPin = 5;
const byte controlEmptyPin = 6;
const byte controlStopPin = 7;
const byte controlFillPin = 8;
const byte servoPin = 9;
const byte pressurePin = A0;
const byte speedControlPin = A2;






unsigned long fiveSecondCounter = 0;

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
}

void pressureToString(char* result, int index) {
  char pCh[4];              // Allocate enough space to hold largest possible
  itoa(data.pressure_int[index], pCh, 10);
  //dtostrf(pr, 3, 1, pCh);
  strcat(result, pCh);
  itoa(data.pressure_decimal[index], pCh, 10);
  strcat(result, pCh);
}

void getDataString(int index) {
  clear(dataString, dataLength);
  strcat(dataString, "DATA-");
  char dCt[3];
  itoa(index, dCt, 10);
  strcat(dataString, dCt);
  strcat(dataString, ":");
  strcat(dataString, "RA01;UTC=");
  timeToString(dataString, data.hour[index], data.minute[index], data.second[index]);
  // Add pressure to string
  strcat(dataString, ";kpa=");
  char pressureTmp[6];
  pressureToString(pressureTmp, index);
  strcat(dataString, pressureTmp);
}

void getSampleData(){
  dataCount = 1;
  for(int i = 0; i < 3; i++){
    data.hour[i] = 88;
    data.minute[i] = 88;
    data.second[i] = 88;
    data.pressure_int[i] = sampleDataCounter + 1;
  }
  sampleDataCounter++;
}

void broadcastCompanyData() {
  radio.dataAdd(companyData);
  radio.dataAdd("; ");

  if (RTC.isRunning()) {
    char time[10] = "";
    timeToString(time, RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());
    radio.dataAdd(time);
  } else {
    radio.dataAdd("TimeError");
  }
  radio.dataAdd(" UTC; ");

 float pressure = readPressure();
    char pCh[4];              // Allocate enough space to hold largest possible
    itoa(int(pressure), pCh, 10);
    radio.dataAdd(pCh);
    radio.dataAdd(".");
    itoa(static_cast<int>((pressure - static_cast<int>(pressure)) * 10), pCh, 10);
    radio.dataAdd(pCh);
  radio.dataAdd(" kpa; ");

  float depth = float(pressure/kpa_to_m);
  char dCh[4];
  itoa(int(depth), dCh, 10);
  radio.dataAdd(dCh);
  radio.dataAdd(".");
  itoa(static_cast<int>((depth - static_cast<int>(depth)) * 10), dCh, 10);
  radio.dataAdd(dCh);
  radio.dataAdd(" meters");

  radio.dataSend();
  radio.waitSent();
}

#endif
