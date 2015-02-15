/* This sketch sents values temperature, humidity, CO2 and the digital filter setting to the serial port. After uploading, open the serial monitor (ctrl+shift+m) to see those.

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

#include <SoftwareSerial.h>
#include "cozir.h"

SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

void setup()
{
  Serial.begin(9600); 
  delay(5000); // The sensor needs a few seconds to calibrate.
}

void loop()
{
  delay(5000); // The number of milliseconds between each readout sent to the serial port. The sensor samples every 1200 ms.
  float t = czr.Celsius();
  float f = czr.Fahrenheit();
  float h = czr.Humidity();
  int c = czr.CO2();
  int digi = czr.GetDigiFilter();

  Serial.print("Celsius : ");Serial.println(t);
  Serial.print("Fahrenheit : ");Serial.println(f);
  Serial.print("Humidity : ");Serial.println(h);
  Serial.print("CO2 : ");Serial.println(c);
  Serial.print("Digital Filter : ");Serial.println(digi); 
  Serial.println("");
}

