#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Streaming.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <TM1638plus.h> // Include the library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h> // Include library for working with JSON

#define STROBE_TM 14 // Strobe = GPIO connected to strobe line of the module
#define CLOCK_TM 12  // Clock = GPIO connected to clock line of the module
#define DIO_TM 13    // Data = GPIO connected to data line of the module

#define OLED_RESET -1
#define OLED_SCREEN_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool high_freq = false;
TM1638plus tm(STROBE_TM, CLOCK_TM, DIO_TM, high_freq);
WiFiClient wifiClient;
byte buttons;

const int JSON_CAPACITY = 1024; // JSON buffer size
const int ARRAY_SIZE = 6;       // Array size to store JSON data

void setup() {
  Serial.begin(115200);
  tm.displayBegin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  WiFi.begin("VM7342892", "w6hkNcgvdc8t");
  Serial.println("Hi there!");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.clearDisplay();
    display << "Connecting to Wi-Fi..." << endl;
    display.display();
    display.setCursor(0, 0);
  }

  display.setCursor(0, 0);
  delay(1000);
  display.clearDisplay();
  display << "Connected to Wi-Fi" << endl;
  display.display();
  tm.displayBegin();
  tm.reset();
  tm.setLEDs(0x0100);
}

void loop() {
  int sensorValue = analogRead(A0);
  Serial << sensorValue << endl;
  tm.reset();
  buttons = tm.readButtons();
  tm.displayIntNum(buttons);

  if (WiFi.status() == WL_CONNECTED) {
    String json = httpRequestAPI();
    Serial.println(json);
    if (json != "") {
      float data[ARRAY_SIZE];
      parseJSON(json, data);
    }

    delay(30000);
  }
}

String httpRequestAPI() {
  HTTPClient http;
  String payload = ""; // Declare payload outside the if block

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Making HTTP request...");
  display.display();

  Serial.println("Making HTTP request...");

  http.begin(wifiClient, "http://api.openweathermap.org/data/2.5/weather?q=Dundee&appid=7bd4c1a3770fadf28a2bc466ad637d3b&units=metric");
  int httpCode = http.GET();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    payload = http.getString(); // Assign response body to payload
    Serial.println("Response payload:");
    //Serial.println(payload);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Response payload:");
    display.display();

    int displayLength = payload.length();
    int displayWidth = 128; // Assuming a line on your display is 128 characters wide
    int numOfLines = displayLength / displayWidth;
    for (int i = 0; i <= numOfLines; ++i) {
      int endIndex = min((i + 1) * displayWidth, displayLength);
      String finalString = payload.substring(i * displayWidth, endIndex);
      //display.println(finalString);

      display.display();
      delay(1000); // Add a delay between chunks for readability (optional)
    }
  } else {
    Serial.println("Failed to connect or receive response");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Failed to connect or receive response");
    display.display();
  }

  http.end();
  return payload; // Return the response payload
}


void parseJSON(String jsonString, float* dataArray) {
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
  }
}

