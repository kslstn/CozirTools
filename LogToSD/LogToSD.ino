/*
  Log environmental data to CSV file on SD card using a DataLogger Shield.

  created 23 Feb 2015
  by Koos Looijesteijn
 
  This code is in the public domain.
 	 
  Upon first use, the sensor must be calibrated. If you don't get proper
  readings, do this: 1) Upload the SetToPolling sketch - Because the library
  is made for polling. 2) Move the sensor to an area with fresh air 3) Upload
  the Calibrate sketch 4) Upload this sketch again.
  
  For logging in Fahrenheit, just replace all instances of 'Celsius'.
*/

/****************************
 Libraries
/****************************/
#include <SD.h>
#include <Wire.h>
#include <ctype.h> // For char to string conversion
#include <RTClib.h> // Real Time Clock https://github.com/jcw/rtclib
#include <stdlib.h> // For float to string conversion
#include <Average.h> // Calculates average, high and low values in arrays
#include <SoftwareSerial.h> // For serial port communication
#include "cozir.h" // Makes it easy to poll data from the sensor


/****************************
 Variables and settings
/****************************/
RTC_DS1307 RTC;

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 10;

// Set serial connection
SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

// We poll more often than we log, so we can register some extremes occurring during the logging intervals without having to log huge amounts of data.
const int LOG_INTERVAL = 3; //60*60; // Seconds between each log. TODO
const byte POLLS_PER_LOG = 2; // Number of polls per logged entry. Byte -> max 7.
const int POLLING_INTERVAL = LOG_INTERVAL / POLLS_PER_LOG; // In seconds. This number can stay high. It takes about 30s for big changes even to be picked up as a small difference by the sensor.

// Special declaration for variables that are averaged with the Average library.
Average<float> temperatures(POLLS_PER_LOG);
Average<float> humidity(POLLS_PER_LOG);
Average<int> CO2(POLLS_PER_LOG);

/****************************
 Setup
/****************************/
void setup(){
  Serial.begin(9600); // Start communication over serial port.
  Wire.begin();
  RTC.begin();
  pinMode(10, OUTPUT);

  if (!SD.begin(chipSelect)) { // see if the card is present and can be initialized:
    Serial.println(F("Card failed, or not present"));
    return; // don't do anything more.
  }

  delay(1000); // TODO
}

/****************************
 Loop
/****************************/
void loop(){
  static unsigned long pollMoment;
  static byte polls;

  if (millis() >= pollMoment + (unsigned long)POLLING_INTERVAL * 1000UL) { // Checking clock with millis() is more accurate than counting loops. Because the numbers can get large, they are converted to UL.
    pollMoment = millis(); // millis() returns the number of milliseconds that passed since the processor started.
    pollData();
    polls++;
  }
  if (polls == POLLS_PER_LOG){
    logData();
    polls = 0;
  }
  delay(100); // Delay prevents serial commands getting chopped up in separate characters.
}

/****************************
 Functions called from Loop
/****************************/

// Converts floats into strings.
String floatToString(float f){
  char buff[10];
  String s = "";
  dtostrf(f, 4, 1, buff);  // Convert float to string (variable, mininum width, precision, buffer)
  s += buff;
  return s;
}
// Polling - requesting measurements from the sensor
void pollData(){
  // Measurements are pushed in arrays for each measurement type:
  temperatures.push(czr.Celsius());
  humidity.push(czr.Humidity());
  CO2.push(czr.CO2());
  return;
}
// Logging of measurements
void logData(){
  File dataFile;
  dataFile = SD.open("datalog.csv", FILE_WRITE);// Create file if it doesn't already exist.
  dataFile.close();
  dataFile = SD.open("datalog.csv"); // Open the file
  if (dataFile) {
    String content = getFirstWord(dataFile);
    dataFile.close();
    if (content.length() == 0){
      addTableHeader(dataFile);     
    }
    if (content == "Time"){
      logToSD(dataFile);
    }
    if ((content != "Time") && (content.length() > 0)){
      Serial.println(F("Something's wrong with datalog.csv"));
    }
  }
  else {
    // if the file didn't open, print an error:
    Serial.println(F("error opening datalog.csv"));
  }
  return; // Close the function to free up memory.
}
String getFirstWord(File dataFile){
  char chars[4];
  String content;
  byte i = 0;
  while( i < 4 ){
    chars[i] = dataFile.read();
    int j = chars[i];
    if (j != -1){ // When end of file is reached, -1 is returned.
      content += String(chars[i]);
    }
    i++;
  }
  return content;
}
void addTableHeader(File dataFile){
  dataFile = SD.open("datalog.csv", FILE_WRITE);
  dataFile.println(F("Time,Temperature (low),Temperature,Temperature (high),Humidity (low),Humidity,Humidity (high),CO2 (low),CO2,CO2 (high)"));
  dataFile.close();
  return;
}
void logToSD(File dataFile){
  dataFile = SD.open("datalog.csv", FILE_WRITE);

  DateTime now = RTC.now();

  if ( now.day() < 10 ){
    dataFile.print(F("0"));
  }
  dataFile.print(now.day(), DEC);
  dataFile.print(F("-"));
  if ( now.month() < 10 ){
    dataFile.print(F("0"));
  }
  dataFile.print(now.month(), DEC);
  dataFile.print(F("-"));
  dataFile.print(now.year(), DEC);
  dataFile.print(F(" "));
  if ( now.hour() < 10 ){
    dataFile.print(F("0"));
  }  
  dataFile.print(now.hour(), DEC);
  dataFile.print(F(":"));
  if ( now.minute() < 10 ){
    dataFile.print(F("0"));
  }  
  dataFile.print(now.minute(), DEC);
  dataFile.print(F(":"));
  if ( now.second() < 10 ){
    dataFile.print(F("0"));
  }
  dataFile.print(now.second(), DEC);
  dataFile.print(F(","));
  dataFile.print(floatToString(temperatures.minimum()) + ",");  
  dataFile.print(floatToString(temperatures.mean()) + ",");  
  dataFile.print(floatToString(temperatures.maximum()) + ",");  
  dataFile.print(floatToString(humidity.minimum()) + ",");  
  dataFile.print(floatToString(humidity.mean()) + ",");  
  dataFile.print(floatToString(humidity.maximum()) + ",");  
  dataFile.print(floatToString(CO2.minimum()) + ",");  
  dataFile.print(floatToString(CO2.mean()) + ",");
  dataFile.print(floatToString(CO2.maximum()) + "\r\n"); 
  
  dataFile.close();
  return;
}