#ifndef DATA_h
#define DATA_h

#include "definitions.h"

        struct dataArray {                        // Allocate storage for recording data
          uint8_t hour[dataLimit];  //: 6
          uint8_t minute[dataLimit];  //: 6
          uint8_t second[dataLimit];  //: 6
          uint8_t pressure_int[dataLimit];  //: 8
          uint8_t pressure_decimal[dataLimit]; //: 4
          uint8_t recieved[dataLimit];  //: 2
        };


float readPressure() {
  /* Sensor returns linear 0.5 to 4.5 V, 0 to 100 PSI
     Maps input values from lower / upper analog read limit to scale of 1000, which will equal tenths of PSI
     Multiplies by 689.5 to convert to PA
  */
  //return map(analogRead(pressurePin), pressureRange[0], pressureRange[1], 0, 100);
  // Round it to one decimal place
  return float(round(float(0.689476 * map(analogRead(pressurePin), pressureRange[0], pressureRange[1], 0, 1000))*10))/10; // 1 psi = 6894.76 pa
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

void Float::resetSampleData(){
  sampleDataCounter = 0;
}

void Float::recordData(){
  if(RTC.isRunning()){
    data.hour[dataCount] = static_cast<uint8_t>(RTC.getHours());
    data.minute[dataCount] = static_cast<uint8_t>(RTC.getMinutes());
    data.second[dataCount] = static_cast<uint8_t>(RTC.getSeconds());
  } else {
    data.hour[dataCount] = static_cast<uint8_t>(42);
    data.minute[dataCount] = static_cast<uint8_t>(42);
    data.second[dataCount] = static_cast<uint8_t>(42);
  }

  float pressure = readPressure();

  data.pressure_int[dataCount] = static_cast<uint8_t>(int(pressure));
  data.pressure_decimal[dataCount] = static_cast<uint8_t>((pressure - static_cast<int>(pressure)) * 10);
  data.recieved[dataCount] = static_cast<uint8_t>(0);
  dataCount++;
}

void Float::sendData(char* values){

}

##endif
