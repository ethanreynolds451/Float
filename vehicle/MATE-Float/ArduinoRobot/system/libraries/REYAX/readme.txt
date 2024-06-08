Instructions for use of REYAX library

The REYAX library allows a REYAX LoRa radio module to be configured from and used with an Arduino microcontroller. This library makes use of the softwareSerial library for communication with the radio module, and includes functions to easily configure radio parameters and exchange data between modules. This library is designed to facilitate easy implementation of a REYAX module into Arduino projects. 


	Library Setup Functions

These functions are used to configure and initiate the REYAX object and set higher-level variables used in program operation. 


REYAX reyax(TX, RX);
	Default = (2, 3)

Initiate REYAX object with softwareSerial on specified pins


reyax.setPins(TX, RX, baud);		
	Default = (2, 3, 9600)

This allows the softwareSerial pins to be changed at any time during the run of the program. This can be used, for example, if the REYAX library is included in another library where the object needs to be defined before it is configured. If this function is called after the port has been opened, it will automatically close the port and reopen it on the new pins. An optional baud rate can be supplied if needed to be changed.


reyax.begin(baud_rate);					
	Default = 9600

Starts communication with the radio module at the specified baud rate.


reyax.setTimeout(timeout);				
	Default = 1 second

Changes the default timeout for radio communication functions. 


reyax.setResponseTimeout(timeout);			
	Default = 3 seconds

Changes the default timeout for functions that wait for a response from a receiving radio.


reyax.setSendLength(length);				
	Default = 46 bytes

Changes the maximum number of characters DATA the radio can send. The default is 46 bytes because this is how much will fit in a 64 byte Arduino softwareSerial receive buffer while accomadating the maximum possible header length. 


reyax.setRecieveLength(length);				
	Default = 64 bytes

Set the maximum number of TOTAL characters (including header) that the radio can receive. Set to match Arduino softwareSerial receive buffer by default. 


reyax.setSendAddress(address);					
	Default = 1

Change the recieve address to be included in the data header. The recieve address is the network address of the recieving module. The module will only read the data it receives if the address in the header matches its own address. An address of zero means the data will be read by all modules on the network. 



	Error Code Functions

Errors are reported by library functions when something goes wrong. If an error is returned by a member function, it will be stored until it is resolved or a new error is reported. The current error can be retrieved using the functions included below. Many functions will return "false" if they fail to execute properly, but these functions must be used to see the specific error. 


byte errorNumber();

Returns the number of the active error.


char* errorCode();

Returns the error code of the active error.


char* errorMessage();

Returns the message associated with the active error.



	Communication Functions

These functions allow the radio to be used to easily send and receive data. The REYAX module requires specific headers to be included with data transmissions and includes a header with extra information for received data. The included functions allow data to be sent and read directly by automatically including the required header and parsing the received data from other included information. 


int reyax.available();

Returns the total number of bytes in the radio serial buffer. This could consist of a data transmission, a status response, or an error message. 


reyax.send(char);

Sends data from a radio module. Accepts character array data type. Data will be truncated if longer than specified maximum send length. 


bool reyax.read(char*, size); 

Mainly for internal use. Reads all data from the serial buffer into a character array provided to the function. Treats character array buffer as pointer and takes size parameter to ensure that data read does not exceed buffer length. Returns false if there is a read error. 


bool reyax.readline(char*, size);

Same as "read" function except stops at newline character. Useful for distinguishing separate radio transmissions since each ends with a newline character. 


bool reyax.readData(char*, size);

This function reads only the data portion of an incoming data transmission. Data is read into a character array provided as a pointer, with the size provided to prevent overflow.  


bool waitSent(timeout);

Waits until the radio has finished sending the data. Returns true if data sent successfully, false if error. If no timeout is provided, timeout will be set to the parameter defined by "setTimeout()";


bool waitRecieved(timeout);

Waits until the response defined by "setConfirmation()" is received. Returns true if confirmation is received before timeout, false if not or if error. If no timeout is provided, timeout will be set to the parameter defined by "setResponseTimeout()";


bool reyax.status();

Returns the status of the REYAX module. True if good, false if error. 


	Data Management Functions

The REYAX library provides functions to manage sending a packet of data. A buffer to hold data waiting to be sent is created. Individual pieces of data can then be added to or removed from this buffer so that they can all be sent as a single packet. 


int dataLength();

Returns the length of the data stored in the buffer. 


bool dataAdd(char*);

Append a new piece of data to the packet. Returns true if successful, false if error. 


dataClear();

Removes all data from the from the buffer. 


char* dataGet();

Creates a character array containing the data in the buffer and returns a pointer to this array. 


bool dataSend();

Sends the data in the buffer then deletes it. 


bool dataSendKeep();

Send the data in the buffer without deleting it. 



	Radio Setup Functions

This set of functions allows the REYAX radio to be configured directly from an Arduino. The functions take configuration parameters, then relay them to the radio module with the proper header and error checking. The setup sketch in the examples folder can be used to configure the REYAX radio over the serial monitor. 

***The setup sketch has not yet been developed***

***Setup functions have not yet been developed***























