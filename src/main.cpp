/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
*********/

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SPIFFS.h"
#include <string>
#include <vector>

bool intervalChanged = false; // Flag to indicate if the interval has changed

// Data wire is connected to GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Define a structure to store temperature readings and timestamps
struct TemperatureReading
{
  String timestamp;
  float temperature;
};

// Vector to store temperature readings
std::vector<TemperatureReading> temperatureReadings;

// Function to get the current timestamp as a String
String getCurrentTimestamp()
{
  // Assuming you want a simple timestamp. Customize as needed.
  unsigned long now = millis();
  return String(now);
}

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// Variables to store temperature values
String temperatureF = "";
String temperatureC = "";
float temperatureCFloat;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// Replace with your network credentials
const char *ssid = "TN-EK6975";
const char *password = "RootjoHoxic3";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Modify readDSTemperatureC to record readings
String readDSTemperatureC()
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  }
  else
  {
    // Use the function to get the current timestamp
    String timestamp = getCurrentTimestamp();
    temperatureReadings.push_back({timestamp, tempC});
    return String(tempC);
  }
}

// Replaces placeholder with DS18B20 values
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "TEMPERATUREC")
  {
    return temperatureC;
  }
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println();

  // Start up the DS18B20 library
  sensors.begin();

  temperatureC = readDSTemperatureC();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });
  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", temperatureC.c_str()); });
  server.on("/getInterval", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(200, "text/plain", String(timerDelay / 1000.0)); // send interval in seconds
            });

  server.on("/increaseInterval", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  timerDelay += 10000; // Increase interval by 10 seconds
  intervalChanged = true; // Set the flag
  request->send(200, "text/plain", "Interval increased"); });

  server.on("/decreaseInterval", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  if (timerDelay > 10000) { // Prevent interval from going below 10 seconds
    timerDelay -= 10000; // Decrease interval by 10 seconds
    intervalChanged = true; // Set the flag
  }
  request->send(200, "text/plain", "Interval decreased"); });
  // Endpoint to download data as CSV
  server.on("/downloadCSV", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String csvData = "Timestamp,Temperature\n";
    for (const auto& reading : temperatureReadings) {
        csvData += reading.timestamp + "," + String(reading.temperature) + "\n";
    }
    request->send(200, "text/csv", csvData); });

  // Endpoint to reset the interval and clear the chart
    server.on("/resetInterval", HTTP_GET, [](AsyncWebServerRequest *request) {
    timerDelay = 10000; // Reset to default interval (e.g., 10 seconds)
    temperatureReadings.clear(); // Clear the temperature readings vector
    request->send(200, "text/plain", "Interval and chart data reset");
});
  // Start server
  server.begin();
}

void loop()
{
  // Check if the interval has changed or the timer has elapsed
  if (intervalChanged || (millis() - lastTime > timerDelay))
  {
    if (intervalChanged)
    {
      intervalChanged = false; // Reset the flag
      lastTime = millis();     // Reset the timer
    }
    else
    {
      temperatureC = readDSTemperatureC(); // Update the temperature
      lastTime = millis();                 // Reset the timer
    }
  }
}
