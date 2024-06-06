#include <Wire.h>
#include <I2C_RTC.h>

static PCF8523 RTC;

void setup() {
      Serial.begin(9600);
      RTC.begin();
  
  // put your setup code here, to run once:

        Serial.println("Setting Time");
        //RTC.setHourMode(CLOCK_H12); //Comment if RTC PCF8563
        RTC.setHourMode(CLOCK_H24);  
        RTC.setDateTime(__DATE__, "10:26:05");
        RTC.updateWeek();           //Update Weekdaybased on new date.    
        Serial.println("New Time Set");
        Serial.print(__DATE__);
        Serial.print(" ");
        Serial.println("Time");
        RTC.startClock(); //Start the Clock;
}

void loop() {
  // put your main code here, to run repeatedly:

}
