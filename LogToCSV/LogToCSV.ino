/*
  Log environmental data to CSV
 
  This sketch sents values temperature, humidity, CO2 and the digital filter setting to the serial port in CSV format.
  After uploading, open the serial monitor (ctrl+shift+m) and follow instructions. The CSV data can be copied and pasted into a spreadsheet.
  In order to keep memory and storage consumption low, but not average out extremes, the sketch polls 30 times as often as it logs. With every log entry, the extremes and the averages of those 30 measurements are recorded.  
 
  created 13 Feb 2015
  by Koos Looijesteijn
 
  This code is in the public domain.
 	 
  Upon first use, the sensor must be calibrated. If you don't get proper readings, do this:
  1) Upload the SetToPolling sketch - Because the library is made for polling.
  2) Move the sensor to an area with fresh air
  3) Upload the Calibrate sketch
  4) Upload this sketch again.
  
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

// Set measure and log intervals
const byte MEASUREMENTS_PER_LOG = 30; // We poll more often than we record, so we can register the extremes without having to log huge amounts of data.
byte pollingInterval = 1; // In seconds.  It takes about 30s for big changes even to be picked up by the sensor, so this number can stay high.
byte pollingLoopCounter = 0;
byte logInterval = MEASUREMENTS_PER_LOG*pollingInterval; // In seconds. This defines how often logs are saved.
byte logLoopCounter = 0;

// Special declaration for variables that are averaged with the Average library.
Average<float> temperatures(MEASUREMENTS_PER_LOG);
Average<float> humidity(MEASUREMENTS_PER_LOG);
Average<int> CO2(MEASUREMENTS_PER_LOG);

// This will contain the measurement and now gets the CSV headings:
String sLog = "Time,Temperature (low),Temperature,Temperature (high),Humidity (low),Humidity,Humidity (high),Carbon Dioxide (high),Carbon Dioxide,Carbon Dioxide (low)\r\n";

// LED indicates when the clock needs to be reset
const byte LED = 13;

/****************************
 Setup
/****************************/
void setup(){
  pinMode(LED, OUTPUT); // LED needs pin set to output
  Serial.begin(9600); // Start communication over serial port.
  //setSyncProvider( requestSync );  //set function to call when sync required
  Serial.println("\r\nPlease enter the current Unix time, for instance: 1423856136\r\n");// You can find the current time stamp on unixtimestamp.com
}

/****************************
 Loop
/****************************/
void loop(){
  
  if (timeStatus() == timeSet) {
    digitalWrite(LED, LOW); // Turn off LED when clock is set
    //TODO klopt dat wel
    if ( pollingLoopCounter == pollingInterval ){
      pollData();
      pollingLoopCounter = 0;
    }
    pollingLoopCounter++;
    if ( logLoopCounter == logInterval ){
      logData();
      logLoopCounter = 0;
    }
    logLoopCounter++;
    if (Serial.available()) {
      processSerialCommandInput();   
    }
  }
  else {
    digitalWrite(LED, HIGH); // Turn on LED when clock needs to be se
    if (Serial.available()) {
      processDateInput();   
    }
  }
  delay(1000);
}

/****************************
 Functions called from Loop
/****************************/
// Process serial monitor commands
void processDateInput() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013    
  pctime = Serial.parseInt();
  Serial.println("- " + String(pctime) + "\r\n");
  if( pctime >= DEFAULT_TIME ){ // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime); // Sync Arduino clock to the time received on the serial port
    Serial.println("Tnx, logging will start. Type 'csv' to get the log data.\r\n");
  }
  else{
    Serial.println("What?");
  }     
}
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
    Serial.println(sLog);
  }  
}
// Polling - requesting measurements from the sensor
void pollData(){
  temperatures.push(czr.Celsius());
  humidity.push(czr.Humidity());
  CO2.push(czr.CO2());
}
// Logging of measurements
void logData(){
  String sTime = getTimeString();
  String sTemperatureMin = floatToString( temperatures.minimum() );
  String sTemperatureAvg = floatToString( temperatures.mean() );
  String sTemperatureMax = floatToString( temperatures.maximum() );
  String sHumidityMin = floatToString( humidity.minimum() );
  String sHumidityAvg = floatToString( humidity.mean() );
  String sHumidityMax = floatToString( humidity.maximum() );
  String sCO2Min = floatToString( CO2.minimum() );
  String sCO2Avg = floatToString( CO2.mean() );
  String sCO2Max = floatToString( CO2.maximum() );
  sLog += sTime + "," + sTemperatureMin + "," + sTemperatureAvg + "," + sTemperatureMax + "," + sHumidityMin + "," + sHumidityAvg + "," + sHumidityMax + "," + sCO2Min + "," + sCO2Avg + "," + sCO2Max + "\r\n" ; // Append all measurements to the string
}
// Returns time as string
String getTimeString(){
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


