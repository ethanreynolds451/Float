#ifndef COMMUNICATION_h
#define COMMUNICATION_h

class Communication {
    public:
        byte failedSendAttempts = 0;
        char dataString[dataLength] = "";         // Create string to hold data
        const int dataInLen = 64;                 // Length of buffer to hold incoming transmissions
        char dataIn[dataInLen];                   // Buffer to hold incomming transmissions

    private:    
        void clear(char* input, int len) {
            for (int i = 0; i < len; i++) {
                input[i] = '\0';
            }
        }
};


void sendTime(){
    clear(dataString, dataLength);
    timeToString(dataString, RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());
    radio.dataAdd("UTC=");
    radio.dataAdd(dataString);
    radio.dataSend();
}
void sendPressure(){
    // clear(dataString, dataLength);
    // pressureToString(dataString, readPressure());
    // radio.dataAdd("kpa=");
    // radio.dataAdd(dataString);

    float pressure = readPressure();

    //Serial.println(pressure);

    data.pressure_decimal[dataCount] = static_cast<int>((pressure - static_cast<int>(pressure)) * 10);

    char pCh[4];              // Allocate enough space to hold largest possible

    itoa(int(pressure), pCh, 10);
    //dtostrf(pr, 3, 1, pCh);
    radio.dataAdd(pCh);

    radio.dataAdd(".");

    itoa(static_cast<int>((pressure - static_cast<int>(pressure)) * 10), pCh, 10);
    radio.dataAdd(pCh);

    radio.dataSend();
}

void transmitData(){
    flag.requestTransmission = false;
    int dataIndex = dataCount;  // Create variable to keep track of total number of data packets
    while(dataIndex >= 0){ // Do this until all the data has been sent
        getDataString(dataIndex);  // Turn the data at specified index into a string with header
        radio.send(dataString); // Send the data over the radio
        if(radio.waitSent(2)){          // Give time to send data
                dataCount--;
        } else {                    // Otherwise, it will send the same data value again
            radio.send(dataString);   // One more chance
            if(! radio.waitSent(2)){
                failedSendAttempts++;
            }
            dataCount--;
        }
    }
}

void sendConfirmation(){
    radio.send("Recieved");
    radio.waitSent();
}

#endif