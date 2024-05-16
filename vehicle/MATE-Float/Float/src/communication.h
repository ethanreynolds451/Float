#ifndef COMMUNICATION_h
#define COMMUNICATION_h

class Communication {
    public:
        int dataCount = 0;                  // Tracks the number of data packets recorded
        int sampleDataCounter = 0;          // Tracks the number of data packets recorded
        dataArray data;                     // Create instance of dataArray named 'data'
        byte failedSendAttempts = 0;


        char dataString[dataLength] = "";         // Create string to hold data
        const int dataInLen = 64;                 // Length of buffer to hold incoming transmissions
        char dataIn[dataInLen];                   // Buffer to hold incomming transmissions
};

#endif