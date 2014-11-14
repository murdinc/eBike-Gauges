// OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display1(OLED_RESET);
Adafruit_SSD1306 display2(OLED_RESET);


void initOLED() {
  
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64) WHITE
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64) BLUE
  
  display1.display(); // show splashscreen
  testdrawline();
  
  delay(1000);

  display1.clearDisplay();   // clears the screen and buffer
  display2.clearDisplay();   // clears the screen and buffer

  setupDisplay1();
  setupDisplay2();
}


void setupDisplay1(void) {
  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
	
  display1.setCursor(0,0);
  display1.print("4:20");
	
  display1.setCursor(62,0);
  display1.print("N");
    
  display1.setCursor(92,0);
  display1.print("Nov 13");
    
  display1.setCursor(4,55);
  display1.print("BAT: 72%");
	
  display1.setCursor(86, 55);
  display1.print("SAT: 4");
	
  display1.setTextSize(4);

  display1.setCursor(14,17);
  display1.print("88");	
	
	
  display1.setTextSize(3);
  display1.setCursor(70,24);
  display1.print("MPH");

  display1.display(); 
}

void setupDisplay2(void) {
  display2.clearDisplay();
  display2.drawLine(0, display2.height()/2, display2.width(), display2.height()/2, WHITE);
  display2.drawLine(display2.width()/2, 0, display2.width()/2, display2.height(), WHITE);
  display2.display();
      	
  display2.drawLine(0, display2.height()/2, display2.width(), display2.height()/2, WHITE);
  display2.drawLine(display2.width()/2, 0, display2.width()/2, display2.height(), WHITE);
      
  delay(250);
}


void testdrawline() {  
  display2.clearDisplay();
  for (int16_t i=0; i<display2.height(); i+=4) {
    display2.drawLine(display2.width()-1, 0, 0, i, WHITE);
    display2.display();
  }
  for (int16_t i=0; i<display2.width(); i+=4) {
    display2.drawLine(display2.width()-1, 0, i, display2.height()-1, WHITE); 
    display2.display();
  }
}

