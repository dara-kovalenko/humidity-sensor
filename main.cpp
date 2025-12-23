#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <DHT_U.h>

#include "resourses.h"
#include "webserver.h"

#define DHTTYPE DHT11

#define HUMIDITY_PIN 16
#define RELAY_PIN 33

const char *AP_SSID = "ESP32 Kovalenko";
const char *AP_PASS = "12345678";

float limit = 30;

float temperature = 0.0;
float humidity = 0.0;

Config config("Biba", "87654321", AP_SSID, AP_PASS);

SensorServer web(config); 

DHT_Unified dht(HUMIDITY_PIN, DHTTYPE);

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);

  WiFi.mode(WIFI_AP);
  Serial.begin(9600);
  dht.begin();
  
  web.bindSensors(&temperature, &humidity);
  
  web.begin(INDEX_HTML);
}

void readSensor()
{
  sensors_event_t event;
  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    temperature = event.temperature;
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    humidity = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("%"));
  }

  if (temperature >= limit) {
    digitalWrite(RELAY_PIN, HIGH);
  } else if (temperature < 28) {
    digitalWrite(RELAY_PIN, LOW);
  }
}

void loop()
{
  static unsigned long lastRead = 0; 

  if (millis() - lastRead > 3000)
  {
    readSensor();
    lastRead = millis();
  }
}