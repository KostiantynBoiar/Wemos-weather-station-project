#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <JsonStreamingParser.h>
#include <JsonListener.h>

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
WiFiClient wifiClient;
byte buttons;

void setup() {
        Serial.begin(115200);
        tm.displayBegin();
        display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
        display.display();
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1); // - a line is 21 chars in this size
        display.setTextColor(WHITE);
        // Настройка WiFi (замените на ваши значения SSID и пароль)
        WiFi.begin("VM7342892", "w6hkNcgvdc8t");
        Serial.println("Hi there!");
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
          tm.displayBegin();
          tm.reset();
          // set up a pattern on the LEDs
          tm.setLEDs(0x0100);

}
void loop() {
  
  int sensorValue = analogRead(A0);
  Serial << sensorValue << endl; 
  tm.reset();
  buttons = tm.readButtons(); 
  tm.displayIntNum(buttons);
  /*
  if(buttons == 1){
      if (WiFi.status() == WL_CONNECTED) {
      httpRequestAPI();
    }
  }
  if(buttons == 2){
    display.clearDisplay();
    display.setCursor(0, 0);
    String payload = "Weather station is not doing any queties on the API";
    Serial.println(payload);
    display << payload << endl;
    display.display();

  }
  */
  if (WiFi.status() == WL_CONNECTED) {
      httpRequestAPI();
      delay(30000);
    }

}

// Function to handle parsed JSON data
void handleData(JsonStreamingParser::EventType evt, JsonObject obj, int curDepth) {
  switch (evt) {
    case JsonStreamingParser::WaitingForEvent:
      break;
    case JsonStreamingParser::StartDocument:
      break;
    case JsonStreamingParser::EndDocument:
      break;
    case JsonStreamingParser::StartObject:
      break;
    case JsonStreamingParser::EndObject:
      break;
    case JsonStreamingParser::StartArray:
      break;
    case JsonStreamingParser::EndArray:
      break;
    case JsonStreamingParser::KeyValuePair:
      // Когда найдена пара ключ-значение
      // Проверяем, является ли ключ "temp"
      if (strcmp(obj.get<char*>(), "temp") == 0) {
        // Получаем значение ключа "temp" в виде числа с плавающей точкой
        obj = obj.next();
        float temperature = obj.getFloat();
        // Выводим температуру в последовательный порт и на OLED-дисплей
        Serial.print("Temperature: ");
        Serial.println(temperature);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Temperature:");
        display.println(temperature);
        display.display();
      }
      // Добавьте обработку других ключей, аналогичную "temp"
      break;
  }
}

void httpRequestAPI() {
  HTTPClient http;
  
  // Отображаем сообщение о выполнении HTTP-запроса
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Making HTTP request...");
  display.display();
  
  Serial.println("Making HTTP request...");
  
  http.begin(wifiClient, "http://api.openweathermap.org/data/3.0/onecall?lat=33.44&lon=-94.04&appid=7bd4c1a3770fadf28a2bc466ad637d3b");
  int httpCode = http.GET();
  
  Serial.print("HTTP Code: ");
  Serial.println(httpCode);
  
  if (httpCode > 0) {
    WiFiClient stream = http.getStream(); // Получаем поток ответа
  
    JsonStreamingParser parser;
    parser.setListener(handleData);
    
    while (stream.available()) {
      parser.parse(stream.read());
    }
  } else {
    Serial.println("Failed to connect or receive response");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Failed to connect or receive response");
    display.display();
  }
  
  http.end();
}
