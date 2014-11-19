#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "TinyGPS.h"

#include "Gauges.h"

////////////////////////////////////////////////////////////////////
//
//OLED
#define DISPLAY1 0x3C
#define DISPLAY2 0x3D

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//GYRO
#define FREQ 30.0 // sample freq in Hz
#define MPU 0x68 // I2C address of the MPU-6050
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// GPS

TinyGPS gps;
////////////////////////////////////////////////////////////////////
// eBike Gauges - Main
// OLED


Gauges gauges;

void setup() {
  gauges.initOLD();
  gauges.initGPS();
  gauges.initGyro();

}


void loop() {

    gauges.getGPSData();



}


class Gauges {
    public:
      Gauges();
      // Inits
      void initOLD();
      void initGPS();
      void initGyro();
      void getGPSData();
      void readAccel();
      void drawDisplay1();
      void drawDisplay2();
    
    private:
      
      // Display
      void logoScreen1();
      void animateScreen2();

      void printTime();
      void printDate();
      void printGyro();
      void printSats();
      void printSpeed();
      void smartdelay(unsigned long ms);
      void screenSelect(uint8_t address, bool fill);
      
      // GPS
      int _year;
      byte _month, _day, _hour, _minute, _second;
      bool _blink;
      byte _tz;
      char _time[8];
      char _date[10];
      unsigned long _age;
      unsigned long _start;
      unsigned long _sats;
      unsigned long _speed;

      // GYRO
      int ampX;
      int ampY;
      int error;
      uint8_t sample_div;
      
      void amplitude(int val, int width, int *amp);
      int i2c_read(int addr, int start, uint8_t *buffer, int size);
      int i2c_write(int addr, int start, const uint8_t *pData, int size);
      int i2c_write_reg(int addr, int reg, uint8_t data);
      
      

};


Gauges::Gauges()
  :  _blink(false)
  ,  _tz(4)
{

  
}

void Gauges::initOLD()
{
  // Initializing a single display and sharing a buffer since having two would take up more memory than we have.
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY2);
  //display.setRotation(2);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY1);
  //display.setRotation(2);
  
  logoScreen1();
  animateScreen2();
  delay(1000); 
  
  drawDisplay1();
  drawDisplay2();
}


void Gauges::initGPS()
{
  Serial.begin(9600);
  _start = millis();
}


void Gauges::initGyro()
{
   // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();
  
  // PWR_MGMT_1: // wake up 
  i2c_write_reg (MPU, 0x6b, 0x00);
  
  // CONFIG: // Low pass filter samples, 1khz sample rate
  //i2c_write_reg (MPU, 0x1a, 0x01);
  
  // GYRO_CONFIG: // 500 deg/s, FS_SEL=1 // This means 65.5 LSBs/deg/s
  //i2c_write_reg(MPU, 0x1b, 0x08);
  
  // CONFIG:
  // set sample rate
  // sample rate FREQ = Gyro sample rate / (sample_div + 1)
  // 1kHz / (div + 1) = FREQ  
  // reg_value = 1khz/FREQ - 1
  sample_div = 100 / FREQ - 1;
  i2c_write_reg (MPU, 0x19, sample_div);
}


void Gauges::logoScreen1() {
  Gauges::screenSelect(DISPLAY1, false);
  display.display();
}


void Gauges::animateScreen2() {
  screenSelect(DISPLAY2, true);
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
}


void Gauges::drawDisplay1() {
  Gauges::screenSelect(DISPLAY1, true);

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  
  printTime();
  printDate();
  
  display.setCursor(4,55);
  display.print(F("BAT: "));
  
  printSats();
  printSpeed();
  
  display.setTextSize(3);
  display.setCursor(70,24);
  display.print(F("MPH"));
  
  display.display(); 
}


void Gauges::drawDisplay2() {
  screenSelect(DISPLAY2, true);
  
  display.drawLine(0, display.height()/2, display.width(), display.height()/2, WHITE);
  display.drawLine(display.width()/2, 0, display.width()/2, display.height(), WHITE);
  
  // Black box in middle
  display.fillRect(display.width()/16, display.height()/8, display.width()/16*14, display.height()/8*6, BLACK);
  
  // Sqyare
  
  int x = ampX;
  int y = ampY;
  int maxX = display.width();
  int maxY = display.height();
  
  int buffer = 32;
 
  // TOP LEFT
  int tlX = ( buffer + 10 ) - ( x / 2 );
  int tlY = ( ( maxY - buffer ) - 15 ) - ( y / 2 );

  // TOP RIGHT  
  int trX = ( ( maxX - buffer ) - 10 ) + ( x / 2 );
  int trY = ( ( maxY - buffer ) - 15 ) + ( y / 2 );
   
  // BOTTOM LEFT
  int blX = ( buffer ) + ( x / 2 );
  int blY = ( ( maxY - buffer ) + 15 ) - ( y / 2 );
  
  // BOTTOM RIGHT
  int brX = ( maxX - buffer ) - ( x / 2 );
  int brY = ( ( maxY - buffer ) + 15 ) + ( y / 2 );
  
  //display.drawLine(32-ampX, ((display.height()/2)+ampY)-ampX-16, (display.width()-32)+ampX, ((display.height()/2)-ampY)-ampX-16, WHITE);
  //display.drawLine(32+ampX, ((display.height()/2)+ampY)+ampX+16, (display.width()-32)-ampX, ((display.height()/2)-ampY)+ampX+16, WHITE);
  display.drawLine(tlX, tlY, trX, trY, WHITE);
  display.drawLine(blX, blY, brX, brY, WHITE);
  
  display.drawLine(tlX, tlY, blX, blY, WHITE);
  display.drawLine(trX, trY, brX, brY, WHITE);
  
  //display.drawLine(32, ((display.height()/2)+ampX)+ampY-8, display.width()-32, ((display.height()/2)+ampX)-ampY-8, WHITE);
  //display.drawLine(32, ((display.height()/2)+ampX)+ampY+8, display.width()-32, ((display.height()/2)+ampX)-ampY+8, WHITE);
  
  printGyro();
  display.display();
}


void Gauges::screenSelect(uint8_t address, bool fill) {
  // changeAddr is a custom class function added to handle multiple displays on the same address. 
  display.changeAddr(address);
  if (fill) {
    display.fillRect(0, 0, 128, 64, BLACK);
  }
}


void Gauges::printTime() {
  if(_age != TinyGPS::GPS_INVALID_AGE) {
    display.setCursor(0,0); 
    
    if (_hour >= 24) {
      ++_day;
      _hour -= 24;
    } else if (_hour < 0) {
        --_day;
        _hour += 24;
    }
    
    if ( _hour > 12 ) {
      _hour -= 12;
    }
    
    
    if (_blink)
      sprintf(_time, "%2d:%02d %02d", _hour, _minute, _second);
    else
      sprintf(_time, "%2d %02d %02d", _hour, _minute, _second);
  
    display.print(_time); 
  } else {
      display.setCursor(0,0); 
      display.print("Scanning.."); 
  }
}


void Gauges::printDate() {
  if(_age != TinyGPS::GPS_INVALID_AGE) {
    display.setCursor(66,0);
    sprintf(_date, "%02d/%02d/%02d", _month, _day, _year);
    display.print(_date); 
  } else {
      display.setCursor(66,0);
      display.print("--/--/--"); 
  }
}


void Gauges::printSats() {
  if(_sats != TinyGPS::GPS_INVALID_SATELLITES) {
    display.setCursor(86, 55);
    display.print(F("SAT: "));
    display.print(_sats);
  } else {
    display.setCursor(86, 55);
    display.print(F("SAT: -"));
  }
 
}


void Gauges::printSpeed() {
  display.setTextSize(4);
  if(_speed != TinyGPS::GPS_INVALID_F_SPEED) {
    char speedChar[2];
    sprintf(speedChar, "%d", _speed);
    
    if (strlen(speedChar) == 1 ) 
      display.setCursor(38,17);
    else
      display.setCursor(14,17);
    display.print(speedChar);
  }
}


void Gauges::printGyro() {
  display.setTextSize(1);
  display.setCursor(14,20);
  display.print(ampX); 
 
  display.setCursor(14,37);
  display.print(ampY); 
}


void Gauges::getGPSData() {
  
  if (millis() - _start > 1000) {
    _blink = !_blink;
    _start = millis();
    smartdelay(1000);
  }
}

void Gauges::smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    _sats = gps.satellites();
    while (Serial.available()) {
      if ( gps.encode(Serial.read()) ) {
        gps.simple_datetime(&_year, &_month, &_day, &_hour, &_minute, &_second, &_age);
        
        //_sats = gps.satellites();
        _speed = gps.f_speed_mph();
        
        _hour+=_tz; // Time Zone
        drawDisplay1();
      }
    }
    readAccel();
    drawDisplay2();
      
  } while (millis() - start < ms);
}

void Gauges::readAccel(){
  uint8_t i2cData[4];
  uint8_t error;
  static uint16_t accX, accY;
  
  error = i2c_read(MPU, 0x3b, i2cData, 4);
  if(error!=0)
  return;

  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  
  amplitude(accX, 128, &ampX);
  amplitude(accY, 64, &ampY);
}



void Gauges::amplitude(int val, int width, int *amp) {
  //*amp = ( val * width) / width;
  //if (*amp == width) *amp = width - 1; 
  *amp = ((float)val*16.0/15000.0);
}


int Gauges::i2c_read(int addr, int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);
  if (n != 1)
  return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
  return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(addr, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
  return (-11);

  return (0);  // return : no error
}


int Gauges::i2c_write(int addr, int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);        // write the start address
  if (n != 1)
  return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
  return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
  return (error);

  return (0);         // return : no error
}


int Gauges::i2c_write_reg(int addr, int reg, uint8_t data)
{
  int error;
  
  error = i2c_write(addr, reg, &data, 1);
  return (error);
}
