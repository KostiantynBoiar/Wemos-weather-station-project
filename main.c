#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Streaming.h>
#include <DS3231.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <TM1638plus.h> // Include the library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h> // Include library for working with JSON

#define LED_PIN 2
#define STROBE_TM 14
#define CLOCK_TM 12
#define DIO_TM 13

#define OLED_RESET -1
#define OLED_SCREEN_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool high_freq = false;
TM1638plus tm(STROBE_TM, CLOCK_TM, DIO_TM, high_freq);
WiFiClient wifiClient;
byte buttons = 0;
DS3231 rtc;

const String cities[] = {"Dundee", "London", "Edinburgh", "Warsaw", "Glasgow", "Zaporizhzhia", "Kyiv"};
const int JSON_CAPACITY = 1024;
const int ARRAY_SIZE = 6;

bool century = false;
bool h12Flag;
bool pmFlag;

void setup() {
  Serial.begin(115200);
  rtc.setClockMode(false);
  Serial << (F("\nDS3231 Hi Precision Real Time Clock")) << endl;
  tm.displayBegin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  pinMode(LED_PIN, OUTPUT);
  WiFi.begin("VM7342892", "w6hkNcgvdc8t");
  Serial.println("Hi there!");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.clearDisplay();
    display.println("Connecting to Wi-Fi...");
    display.display();
    display.setCursor(0, 0);
  }

  display.setCursor(0, 0);
  delay(1000);
  display.clearDisplay();
  display.println("Connected to Wi-Fi");
  display.display();
  tm.displayBegin();
  tm.reset();
  tm.setLEDs(0x0100);
}

void loop() {
  Serial << rtc.getDate() << "/" << rtc.getMonth(century) << "/" << rtc.getYear() << " " ;
  Serial << rtc.getHour(h12Flag, pmFlag) << ":" << rtc.getMinute() << ":" << 
  rtc.getSecond() << endl; 
  delay(1000); // do nothing
  int sensorValue = analogRead(A0);
  buttons = tm.readButtons();
  Serial << sensorValue << endl;
  Serial.println(sensorValue);
  float responseDelay = 90000 * sensorValue / 250;
  if (responseDelay == 0) {
    responseDelay = 90000;
  }
  if (responseDelay < 90000) {
    responseDelay = 90000;
  }
  int intRespDelay = round(responseDelay);
  Serial.print("Delay: ");
  Serial.println(intRespDelay);
  tm.reset();
  buttons = tm.readButtons();
  tm.displayIntNum(sensorValue);
  uint16_t leds = 0b011111111;

    Serial.print("Setting LEDs: ");
    if (intRespDelay < 100000) {
        tm.setLED(0, 1);
        tm.setLED(1, 0);
        tm.setLED(2, 0);
        tm.setLED(3, 0);
        tm.setLED(4, 0);
        tm.setLED(5, 0);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else if (intRespDelay > 240000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 1);
        tm.setLED(5, 1);
        tm.setLED(6, 1);
        tm.setLED(7, 1);
    } else if (intRespDelay > 220000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 1);
        tm.setLED(5, 1);
        tm.setLED(6, 1);
        tm.setLED(7, 0);
    } else if (intRespDelay > 200000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 1);
        tm.setLED(5, 1);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else if (intRespDelay > 180000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 1);
        tm.setLED(5, 0);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else if (intRespDelay > 160000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 0);
        tm.setLED(5, 0);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else if (intRespDelay > 140000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 0);
        tm.setLED(4, 0);
        tm.setLED(5, 0);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else if (intRespDelay > 120000) {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 0);
        tm.setLED(3, 0);
        tm.setLED(4, 0);
        tm.setLED(5, 0);
        tm.setLED(6, 0);
        tm.setLED(7, 0);
    } else {
        tm.setLED(0, 1);
        tm.setLED(1, 1);
        tm.setLED(2, 1);
        tm.setLED(3, 1);
        tm.setLED(4, 1);
        tm.setLED(5, 1);
        tm.setLED(6, 1);
        tm.setLED(7, 1);
    }


  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
    float data[ARRAY_SIZE];
    if (buttons != 0) {
      int cityIndex = buttons - 1;
      String json = httpRequestAPI(cities[cityIndex]);
      if (json != "") {
        parseJSON(json, data, cities[cityIndex]);
      }
    } else {
      String json = httpRequestAPI(cities[0]);
      parseJSON(json, data, cities[0]);
    }
  }

  unsigned long start = millis();
  while (millis() - start < intRespDelay) {
    if (tm.readButtons() != 0) {
      break;
    }
    delay(50);
  }
  //delay(10000);
  }


String httpRequestAPI(String city) {
  HTTPClient http;
  String payload = "";

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Making HTTP request...");
  display.display();

  Serial.println("Making HTTP request...");

  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=7bd4c1a3770fadf28a2bc466ad637d3b&units=metric";
  http.begin(wifiClient, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    payload = http.getString();
  } else {
    Serial.println("Failed to connect or receive response");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Failed to connect or receive response");
    display.display();
  }

  http.end();
  return payload;
}


void parseJSON(String jsonString, float* dataArray, String city) {
  DynamicJsonDocument doc(JSON_CAPACITY);

  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extracting data from the JSON object
  float lon = doc["coord"]["lon"]; // Longitude
  float lat = doc["coord"]["lat"]; // Latitude

  const char* weatherMain = doc["weather"][0]["main"]; // Weather main description
  const char* weatherDesc = doc["weather"][0]["description"]; // Weather description

  float temperature = doc["main"]["temp"]; // Temperature
  float feelsLike = doc["main"]["feels_like"]; // Feels like temperature
  float tempMin = doc["main"]["temp_min"]; // Minimum temperature
  float tempMax = doc["main"]["temp_max"]; // Maximum temperature
  float pressure = doc["main"]["pressure"]; // Pressure
  float humidity = doc["main"]["humidity"]; // Humidity

  float visibility = doc["visibility"]; // Visibility
  float windSpeed = doc["wind"]["speed"]; // Wind speed
  float windDeg = doc["wind"]["deg"]; // Wind degree

  int cloudsAll = doc["clouds"]["all"]; // Clouds

  unsigned long sunrise = doc["sys"]["sunrise"]; // Sunrise time
  unsigned long sunset = doc["sys"]["sunset"]; // Sunset time
  const char* country = doc["sys"]["country"]; // Country

  // Use the extracted data as needed
  dataArray[0] = lon;
  dataArray[1] = lat;
  dataArray[2] = temperature;
  dataArray[3] = feelsLike;
  dataArray[4] = tempMin;
  dataArray[5] = tempMax;

  // Print the dataArray to Serial
  for (int i = 0; i < ARRAY_SIZE; i++) {
    Serial.print("Data ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(dataArray[i]);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("City: ");
    display.println(city);

    display.print("Temperature: ");
    display.println(dataArray[2]);

    display.print("Feels like: ");
    display.println(dataArray[3]);

    display.print("Temperature min: ");
    display.println(dataArray[4]);

    display.print("Temperature max: ");
    display.println(dataArray[5]);
    display.display();
  }
}

