// OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define DISPLAY1 0x3C
#define DISPLAY2 0x3D

void initOLED() {
  
  // Initializing a single display and sharing a buffer since having two would take up more memory than we have.
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY2);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY1);
  
  logoScreen1();
  animateScreen2();
  delay(1000); 
  
  drawDisplay1();
  drawDisplay2();
}

static void printDate() {
  if(age != TinyGPS::GPS_INVALID_AGE) {
    char date[8];  
    display.setCursor(66,0);
    sprintf(date, "%02d/%02d/%02d", month, day, year);
    display.print(date); 
  }

}
  
static void printTime() {
  if(age != TinyGPS::GPS_INVALID_AGE) {
    char time[8];
    display.setCursor(0,0); 
    control_blink = !control_blink;
    hour+=TZ; // Time Zone
    
    if (hour >= 24) {
      ++day;
      hour -= 24;
    } else if (hour < 0) {
        --day;
        hour += 24;
    }
    
    if ( hour > 12 ) {
      hour -= 12;
    }
    
    
    if (control_blink)
      sprintf(time, "%2d:%02d %02d", hour, minute, second);
    else
      sprintf(time, "%2d %02d %02d", hour, minute, second);
  
    display.print(time); 
  }
}

static void printSpeed(int val) {
  display.setTextSize(4);
  if(val != TinyGPS::GPS_INVALID_F_SPEED) {
    char speedChar[2];
    sprintf(speedChar, "%d", val);
    
    if (strlen(speedChar) == 1 ) 
      display.setCursor(38,17);
    else
      display.setCursor(14,17);
    display.print(speedChar);
  }
}

static void printSats(int val) {
  if(val != TinyGPS::GPS_INVALID_SATELLITES) {
    display.setCursor(86, 55);
    display.print(F("SAT: "));
    display.print(val);
  }
}


static void printGyro() {
  display.setTextSize(1);
  display.setCursor(14,20);
  display.print(ampX); 
 
  display.setCursor(14,37);
  display.print(ampY); 
}

void drawDisplay1() {
  screenSelect(DISPLAY1, true);

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  
  printTime();
  printDate();
  
  display.setCursor(4,55);
  display.print(F("BAT: "));
  
  printSats(gps.satellites());
  
  printSpeed(gps.f_speed_mph());
  
  display.setTextSize(3);
  display.setCursor(70,24);
  display.print(F("MPH"));
  
  display.display();
  
}

static void logoScreen1() {
  screenSelect(DISPLAY1, false);
  display.display();
}

static void screenSelect(uint8_t address, bool fill) {
  // changeAddr is a custom class function added to handle multiple displays on the same address. 
  display.changeAddr(address);
  if (fill) {
    display.fillRect(0, 0, 128, 64, BLACK);
  }
}


void drawDisplay2() {
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

static void animateScreen2() {
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


