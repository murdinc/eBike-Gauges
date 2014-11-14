// eBike Gauges - Main

// GPS
long latitude;              // Latitude in hundred thousandths of a degree
long longitude;             // Longitude in hundred thousandths of a degree
long altitudeGps;           // Altitude in centimeters
unsigned short satellites;  // Number of satellites tracked
unsigned long fix_age;      // Age of fix in milliseconds
unsigned long date;         // Date as ddmmyy
unsigned long time;         // Time as hhmmsscc
unsigned long time_age;     // Age in milliseconds

// Battery Voltage
long voltage;               // Voltage in 100th of a Volt

void setup() {
  initGPS();
  initOLED();
  initGYRO();
}

void loop()
{
  readGPS();
}
