This is a set of Arduino sketches to make use of the CoZIR gas sensors. These sensors can measure temperature, humidity and CO₂. They are sold by [Gas Sensing Solutions](http://www.gassensing.co.uk/products/). The code is based on and partly copied from a thread on the [Arduino forum](http://forum.arduino.cc/index.php?topic=91467.0), where users DirtGambit and robtillaart did great work making it possible for Arduino users to use these sensors.

# Tools 
### Calibrate
Commands the sensor to calibrate itself. To be used when the sensor is surrounded by fresh air.
### LogToCSV
Polls the sensor and logs average and extreme measurements at fixed intervals. Via the serial monitor (USB) the log can be requested as CSV, that can be copied to a spreadsheet. About nine logs can be made before the memory is full. As soon as I have a datalogger shield, I will make sketch for creating bigger logs.
### LogToSerial
Gets the sensor's measurements and sends these to the serial port.
### SetDigiFilter
The digital filter can be adjusted to filter out noise in the measurements
### SetToPolling
Polling is the preferred mode as it potentially uses less energy. The tools require polling mode.
### SetToStreaming
Streaming is the factory default.

# Setup
- Use the wiring schematic from C02meter.com (see below). The tools use digital pins 3 and 2 instead of the ones shown in that manual. Connect pin 3 to the sensor's Rx pin and pin 2 to the sensor's Tx pin.
- Install the dependencies.

# Documentation
- [Wiring schematic](www.co2meters.com/FDocumentation/FAppNotes/FAN128-/520Cozir_Arduino.pdf) - Link only works if you click the first result of the Google search "Arduino Cozir"
- [CoZIR Software user guide](www.airtest.com/support/datasheet/COZIRSerialInterface.pdf) contains the documentation for the serial interface.

# Dependencies
- The tools use [Rob Tillaart's Cozir Library](https://github.com/RobTillaart/Arduino/tree/master/libraries/Cozir).
- The loggers also need the [Average library](https://github.com/MajenkoLibraries/Average).
- The LogToSD tool requires a DataLogger shield and [this DS1307RTC library](https://www.pjrc.com/teensy/td_libs_DS1307RTC.html).
- The LogToCSV tool for logging without DataShield requires the [Time library](https://github.com/PaulStoffregen/Time)

[To install](arduino.cc/en/Guide/Libraries) libraries, download it to your Arduino/library folder and restart the Arduino IDE. 
