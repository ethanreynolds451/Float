#ifndef DATA_h
#define DATA_h


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

##endif
