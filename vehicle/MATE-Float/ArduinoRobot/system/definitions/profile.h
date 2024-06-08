#ifndef PROFILE_h
#define PROFILE_h

/* Data will be recorded at five second intervals
 *
*/

// Data collection parameters
const int dataLimit = 30;           // Maximum quantity of data values to be recorded on vertical profile. All data storage space is allocated at compilation. Adjust this value to fit Arduino memory
const bool stopAtBottom = 0;        // Float will stop recording data once it reaches the bottom
const bool useLocalTime = 0;        // Adjust weather UTC or Local Time is used
    const int timeZoneOffset = +5;  // Time to be used for time zone correction

// Profile parameters
const int maxTimeUnder = 180;       // Maximum time in seconds float will remain below surface before acivating resurface
const bool useConstantDepth = 1;        // Float will resurface once it reaches a specified depth
    const float profileDepth = 3;      // Depth of profile in meters
    const float profileBuffer = 0.5;   // Window in which float will recognize it has reached bottom / surface
    
// Data transmission parameters
const int defaultBaudrate = 9600;                // For communication with REYAX module
const int dataLength = 64;                       // Define max length in bytes of data packet
const char companyData[dataLength] = "RA06";     // Company number that will be transmitted before vertical profile
const byte dataSendDelay = 100;                  // How many milliseconds to wait between data packet transmission

#endif