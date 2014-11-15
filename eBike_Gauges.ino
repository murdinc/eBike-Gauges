// eBike Gauges - Main
// OLED
#include <SPI.h>
#include <Wire.h>

// GPS
static int year;
static byte month, day, hour, minute, second;
unsigned long age;
static bool control_blink = false;
byte TZ = 4;

//static unsigned short int satellites;  // Number of satellites tracked

// Battery Voltage
//long voltage;               // Voltage in 100th of a Volt

void setup() {
  Serial.begin(9600);
  initOLED();
  initGyro();
  initGPS();
}

void loop()
{
  getGPSData();
  drawDisplay1();
  drawDisplay2();
  readAccel();
}

