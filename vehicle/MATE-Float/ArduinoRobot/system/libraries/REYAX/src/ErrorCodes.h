#ifndef ErrorCodes_h
#define ErrorCodes_h

#include "Arduino.h"

struct errorArray {
  uint8_t errorNumber;
  char errorCode[8];
  char errorMessage[64];
};

//

inline const errorArray error[18] = {
 {0, "+ERR=0", "No Error"},
 {1, "+ERR=1", "There is not “enter” or 0x0D 0x0A in the end of the AT Command."},
 {2, "+ERR=2", "The15 head of AT command is not “AT” string."},
 {4, "+ERR=4", "Unknow command."},
 {5, "+ERR=5", "The data to be sent does not match the actual length"},
 {10, "+ERR=10", "TX is over times."},
 {12, "+ERR=12", "CRC error."},
 {13, "+ERR=13", "TX data exceeds 240bytes."},
 {14, "+ERR=14", "Failed to write flash memory."},
 {15, "+ERR=15", "Unknow failure."},
 {17, "+ERR=17", "Last TX was not completed"},
 {18, "+ERR=18", "Preamble value is not allowed."},
 {19, "+ERR=19", "RX failed, Header error"},
 {20, "+ERR=20", "The time setting value of the “Smart receiving power saving mode” is not allowed."},
 {30, "+ERR=30", "Data exceeds buffer length"},
 {31, "+ERR=31", "No data available"},
 {40, "+ERR=40", "No hardware response: check radio connection and timeout"}, 
 {41, "+ERR=41", "No radio response: check reciever status and reply timeout"}
};

#endif
