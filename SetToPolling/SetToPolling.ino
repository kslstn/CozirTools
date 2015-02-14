/* 
   Sets Cozir Sensor to polling mode.
   Just upload the sketch and leave the Arduino connected for a couple of seconds.
*/
#include <SoftwareSerial.h>
#include "cozir.h"

SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

void setup()
{
  Serial.begin(9600);
  czr.SetOperatingMode(CZR_POLLING);
}
void loop()
{}
