/* 
  Calibrates the Sensor. Just opload it and leave the Arduino connected for a couple of seconds. There are no settings or UI.
  For good measurements, move the sensor to an area with fresh air before uploading the sketch.
*/

#include <SoftwareSerial.h>
#include "cozir.h"

SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

void setup()
{
  Serial.begin(9600);
  czr.CalibrateFreshAir();
}
void loop()
{}
