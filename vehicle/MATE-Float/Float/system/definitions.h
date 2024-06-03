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

defaultBaudrate = 9600;

// *** Pin Definitions ***

class Pin {
    const byte defaultTX = 2;
    const byte defaultRX = 3;
    const byte limitEmpty = 4;
    const byte limitFull = 5;
    const byte controlEmpty = 6;
    const byte controlStop = 7;
    const byte controlFill = 8;
    const byte servo = 9;
    const byte pressure = A0;
    const byte speedControl = A2;
};
Pin pin;

#endif
