// Conditional Compilation for ESP32 and ESP8266
#ifdef ESP32
  // Libraries specific to ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
#else
  // Libraries specific to ESP8266
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif

// Common libraries for both ESP32 and ESP8266
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SPIFFS.h"
#include <string>
#include <vector>

// Global Variables
bool intervalChanged = false; // Flag for interval change
#define ONE_WIRE_BUS 4 // GPIO pin for DS18B20 data wire

// OneWire instance for DS18B20 communication
OneWire oneWire(ONE_WIRE_BUS);

// Structure for temperature readings
struct TemperatureReading {
  String timestamp;
  float temperature;
};

// Vector to store temperature readings
std::vector<TemperatureReading> temperatureReadings;

// Function to get current timestamp as a String
String getCurrentTimestamp() {
  unsigned long now = millis(); // Get system uptime in milliseconds
  return String(now);
}

// Dallas Temperature sensor setup
DallasTemperature sensors(&oneWire);

// Temperature variables
String temperatureF = "";
String temperatureC = "";
float temperatureCFloat;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000; // 10-second default interval

const char *ssid = "TN-EK6975";
const char *password = "RootjoHoxic3";

// AsyncWebServer on port 80
AsyncWebServer server(80);

// Read temperature from DS18B20 sensor
String readDSTemperatureC() {
  sensors.requestTemperatures(); // Send command to get temperatures
  float tempC = sensors.getTempCByIndex(0); // Read temperature in Celsius
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    // Store temperature with timestamp
    String timestamp = getCurrentTimestamp();
    temperatureReadings.push_back({timestamp, tempC});
    return String(tempC);
  }
}

// Processor function for web server placeholders
String processor(const String& var) {
  if (var == "TEMPERATUREC") {
    return temperatureC;
  }
  return String();
}

void setup() {
  // Serial setup for debugging
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize DS18B20 sensor
  sensors.begin();
  temperatureC = readDSTemperatureC();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Print ESP IP Address
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", temperatureC.c_str());
  });

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

void loop() {
  // Regular interval check for temperature update
  if (intervalChanged || (millis() - lastTime > timerDelay)) {
    if (intervalChanged) {
      intervalChanged = false;
      lastTime = millis();
    } else {
      temperatureC = readDSTemperatureC();
      lastTime = millis();
    }
  }
}