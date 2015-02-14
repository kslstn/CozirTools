/* Sets Cozir Sensor's filter. The filter can be set to a value between 1 and 256. Use 0 for a smart filter. A low value will result in the fastest response to changes in gas concentration, a high value will result in a slower response. The factory default is 32.
   For more information, see www.airtest.com/support/datasheet/COZIRSerialInterface.pdf
   Just upload the sketch and leave the Arduino connected for a couple of seconds.
*/
#include <SoftwareSerial.h>
#include "cozir.h"

SoftwareSerial nss(2,3); // Pin 2 = Arduino receiver pin (Rx), connect to sensor transmitter pin (Tx). Pin 3 = Arduino Tx, connect it to sensor Rx.
COZIR czr(nss);

void setup()
{
  Serial.begin(9600);
  czr.SetDigiFilter(64);
}
void loop()
{}
