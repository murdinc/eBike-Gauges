// GPS
#include "TinyGPS.h"
#include <SoftwareSerial.h>

TinyGPS gps;
SoftwareSerial gpsSerial(12, 13);

static void smartdelay(unsigned long ms);

void initGPS()   {    
  gpsSerial.begin(9600); 
}

void getGPSData() {
  
  smartdelay(100);
  gps.simple_datetime(&year, &month, &day, &hour, &minute, &second, &age);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

