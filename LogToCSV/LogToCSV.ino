/*
  Log environmental data to CSV
 
  This sketch sents values temperature, humidity, CO2 and the digital filter
  setting to the serial port in CSV format. After uploading, open the serial
  monitor (ctrl+shift+m) and follow instructions. The CSV data can be copied
  and pasted into a spreadsheet. In order to keep memory and storage
  consumption low, but not average out extremes, the sketch polls more
  frequently than it logs. As the log is kept in RAM, only a around 9 data
  points can be recorded. As opening the serial monitor resets the Arduino,
  logs can only be read if the computer is connected during the whole logging
  period. Depending on your computer, you may put it on standby. Just don't
  reset the serial connection. When memory is full, logs are no longer saved.
  Some messy last entries may show up.

  created 13 Feb 2015
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
#include <stdlib.h> // For float to string conversion
#include <Time.h> // Creates Unix time stamps
#include <Average.h> // Calculates average, high and low values in arrays
#include <SoftwareSerial.h> // For serial port communication
#include "cozir.h" // Makes it easy to poll data from the sensor

/****************************
 Variables and settings
/****************************/
// Set serial connection
SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

// We poll more often than we log, so we can register some extremes occurring during the logging intervals without having to log huge amounts of data.
const int LOG_INTERVAL = 60*60; // Seconds between each log. 
const byte POLLS_PER_LOG = 5; // Number of polls per logged entry.
const int POLLING_INTERVAL = LOG_INTERVAL / POLLS_PER_LOG; // In seconds. This number can stay high. It takes about 30s for big changes even to be picked up as a small difference by the sensor.

// Special declaration for variables that are averaged with the Average library.
Average<float> temperatures(POLLS_PER_LOG);
Average<float> humidity(POLLS_PER_LOG);
Average<int> CO2(POLLS_PER_LOG);

// This will contain the log:
String sLog;

/****************************
 Setup
/****************************/
void setup(){
  Serial.begin(9600); // Start communication over serial port.
  Serial.println("\r\n> Please enter the current Unix time, for instance: 1420070400\r\n");// You can find the current time stamp on unixtimestamp.com
}

/****************************
 Loop
/****************************/
void loop(){
  static unsigned long pollMoment;
  static byte polls;

  if (timeStatus() == timeSet) { // Wait for time stamp before doing anything else
    if ( millis() >= pollMoment + (unsigned long)POLLING_INTERVAL * 1000UL) { // Checking clock with millis() is more accurate than counting loops. Because the numbers can get large, they are converted to UL.
      pollMoment = millis(); // millis() returns the number of milliseconds that passed since the processor started.
      pollData();
      polls++;
    }
    if ( polls == POLLS_PER_LOG ){
      logData();
      polls = 0;
    }
    if (Serial.available()) {
      processSerialCommandInput();   
    }
  }
  else { 
    if (Serial.available()) {
      processDateInput();   
    }
  }
  delay(100); // Delay prevents serial commands getting chopped up in separate characters.
}

/****************************
 Functions called from Loop
/****************************/
// Process date commands
void processDateInput() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013    
  pctime = Serial.parseInt();
  Serial.println("- " + String(pctime) + "\r\n");
  if( pctime >= DEFAULT_TIME ){ // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime); // Sync Arduino clock to the time received on the serial port
    Serial.println("> Thanks. I believe you when you say it's " + getTimeString() +".");
    delay(4000); // Sensor needs a couple of seconds to calibrate.
    Serial.println("> Logging will start now.\r\n");
  }
  else{
    Serial.println("> Is that a time stamp?");
  }     
}
// Process serial monitor commands
void processSerialCommandInput() {
  String userInputMsg = "";
  char character;

  while(Serial.available()) {   
    character = Serial.read();
    userInputMsg.concat(character); // Turn latest serial command into a string.
  }
  if ( userInputMsg != ""){
    Serial.println("- " + userInputMsg + "\r\n");
  }
  if( userInputMsg == "csv" ){ // User requests CSV
    if ( sLog.length() > 0){
      Serial.println("Time,Temperature (low),Temperature,Temperature (high),Humidity (low),Humidity,Humidity (high),Carbon Dioxide (low),Carbon Dioxide,Carbon Dioxide (high)");
      Serial.println(sLog);
    }
    else{
      Serial.println("> No log yet.\r\n");
    }
  }  
}
// Polling - requesting measurements from the sensor
void pollData(){
  // Measurements are pushed in arrays for each measurement type:
  temperatures.push(czr.Celsius());
  humidity.push(czr.Humidity());
  CO2.push(czr.CO2());
}
// Logging of measurements
void logData(){
  // Give feedback when first log is made and can be requsted.
  if (sLog.length() == 0){
    Serial.println("> Type 'csv' to get the log data.\r\n");
  }
  String sTime = getTimeString();
  // Get for temperature, humidity and CO2 the lowest, average and highest measurements of the last logging interval:
  String sTemperatureMin = floatToString( temperatures.minimum() );
  String sTemperatureAvg = floatToString( temperatures.mean() );
  String sTemperatureMax = floatToString( temperatures.maximum() );
  String sHumidityMin = floatToString( humidity.minimum() );
  String sHumidityAvg = floatToString( humidity.mean() );
  String sHumidityMax = floatToString( humidity.maximum() );
  String sCO2Min = floatToString( CO2.minimum() );
  String sCO2Avg = floatToString( CO2.mean() );
  String sCO2Max = floatToString( CO2.maximum() );
  // Append the values to the log variable:
  sLog += sTime + "," + sTemperatureMin + "," + sTemperatureAvg + "," + sTemperatureMax + "," + sHumidityMin + "," + sHumidityAvg + "," + sHumidityMax + "," + sCO2Min + "," + sCO2Avg + "," + sCO2Max + "\r\n" ; // Append all measurements to the string
}
// Returns time as string
String getTimeString(){
  // This is UTC time. You may want to make changes to match your time zone.
  String sTime = String( day() ) + " " + monthShortStr( month() ) + " " + String( year() ) + " " + String( hour() ) + convertDigits( minute() ) + convertDigits( second() );
  return sTime;
}
// Digital time formatting: returns preceding colon and leading 0
String convertDigits(int digits) {
  String d = ":";
  if(digits < 10){
    d += 0;
  }
  d += digits;
  return d;
}
// Converts floats into strings.
String floatToString(float f){
  char buff[10];
  String s = "";
  dtostrf(f, 4, 1, buff);  // Convert float to string (variable, mininum width, precision, buffer)
  s += buff;
  return s;
}