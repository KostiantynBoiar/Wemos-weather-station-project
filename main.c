#include <Wire.h>
#include <Streaming.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TM1638plus.h> //include the library
#define STROBE_TM 14 // strobe = GPIO connected to strobe line of module
#define CLOCK_TM 12 // clock = GPIO connected to clock line of module
#define DIO_TM 13 // data = GPIO connected to data line of module

// -- OLED -------
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// OLED i2c
#define OLED_RESET -1
#define OLED_SCREEN_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true.
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);
uint8_t username[] = {'a', 'b', 'e', 'r', 't', 'e', 'y', 'u', 'n', 'i', 'v', 'e', 'r', 's', 'i', 't', 'y', '_', 'b', 'o', 'i', 'a', 'r', '_', 'k', 'o', 's', 't',' i', 'a', 'n', 't', 'y', 'n', '\0'}; //abertayuniversity_boiar_kostiantyn

void setup() {
        tm.displayBegin();
        display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
        display.display();
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1); // - a line is 21 chars in this size
        display.setTextColor(WHITE);
        WiFi.begin("VM7342892", "w6hkNcgvdc8t");
        while (WiFi.status() != WL_CONNECTED) {
          delay(1000);
          display.clearDisplay();
          display << "Connecting to Wi-Fi..." << endl;
          display.display();
          display.setCursor(0,0);
        }
          display.setCursor(0,0);
          delay(1000);
          display.clearDisplay();
          display << "Connected to Wi-Fi" << endl;
          display.display();

}
void loop() {
  int sensorValue = analogRead(A0);
  Serial << sensorValue << endl; 
  tm.reset();
  for(int i = 0; i < 35; i++){
    tm.displayASCIIwDot(0, username[i]);
    delay(1000);
  }
}
