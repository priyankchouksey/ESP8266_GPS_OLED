#include <TinyGPS++.h>                                  // Tiny GPS Plus Library
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module

#include <Wire.h>                                       // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"                                // legacy include: `#include "SSD1306.h"`

static const int RXPin = 00, TXPin = 16;                // Ublox 6m GPS module to pins 12 and 13
static const uint32_t GPSBaud = 9600;                   // Ublox GPS default Baud Rate is 9600
const double Home_LAT = 30.0347201;                      // Your Home Latitude
const double Home_LNG = -95.6531897;                     // Your Home Longitude

static const int fetchDelay = 200;

TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(RXPin, TXPin);                        // The serial connection to the GPS device

// Initialize the OLED display using Wire library
// D1/GPIO5 -> SCK/SCL
// D2/GPIO4 -> SDA
SSD1306Wire  display(0x3c, 4, 5);

void setup() {
  Serial.begin(115200);
  delay(1500);                                          // Pause 1.5 seconds  
  ss.begin(GPSBaud);                                    // Set Software Serial Comm Speed to 9600    
  
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

}

void loop() {
  // Dump raw data from GPS
  byte rawData = ss.read();
  Serial.write(rawData);
  
  int line = 0;
  int lineheight = 10;
  display.clear();
  String concatStr;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line, "Latitude  : ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, String(gps.location.lat(), 6));

  line += lineheight;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line,"Longitude : ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, String(gps.location.lng(), 6));
  
//  line += lineheight;
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.drawString(0, line, "Satellites : ");
//  display.setTextAlignment(TEXT_ALIGN_RIGHT);
//  display.drawString(128, line, String(gps.satellites.value()));
//  
//  line += lineheight;
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.drawString(0, line, "Elevation : ");
//  concatStr = String(gps.altitude.feet(), 0) + "ft";
//  display.setTextAlignment(TEXT_ALIGN_RIGHT);
//  display.drawString(128, line, concatStr);
//  
  line += lineheight;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line, "Time UTC : ");
  concatStr = String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()); // GPS time UTC 
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, concatStr);
  
  line += lineheight;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line, "Heading   : ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, String(gps.course.deg(), 1));
  
  line += lineheight;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line, "Speed     : ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, String(gps.speed.mph(), 0) + " mph");
  
  line += lineheight;
  unsigned long Distance_To_Home = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),Home_LAT, Home_LNG);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line, "KM to Home: ");                        // Have TinyGPS Calculate distance to home and display it
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, line, String(Distance_To_Home, 2));


  delay(fetchDelay); 
  smartDelay(500);                                      // Run Procedure smartDelay
  if (millis() > 5000 && gps.charsProcessed() < 10)
    display.drawString(0,10,"Waiting for GPS Signal...");
  display.display();
}
static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
 
