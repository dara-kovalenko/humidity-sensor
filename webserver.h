#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

struct Config
{
    const char *staSsid;
    const char *staPass;
    const char *ap_ssid;
    const char *ap_pass;
    const uint32_t timeout;
    const uint16_t port;

    Config(const char *staSsid, const char *staPass, const char *ap_ssid, const char *ap_pass, const uint32_t timeout = 10000, const uint16_t port = 80)
        : staSsid(staSsid), staPass(staPass), ap_ssid(ap_ssid), ap_pass(ap_pass), timeout(timeout), port(port)
    {
    }

    Config(const char *ap_ssid, const char *ap_pass, const uint32_t timeout = 10000, const uint16_t port = 80)
        : Config("", "", ap_ssid, ap_pass, timeout, port)
    {
    }
};

class SensorServer
{
private:
    Config config;
    AsyncWebServer server;
    
    float* tempPtr = nullptr;
    float* humPtr = nullptr;

public:
    explicit SensorServer(const Config &cfg)
        : config(cfg), server(cfg.port)
    {
    }

    void bindSensors(float* t, float* h) {
        tempPtr = t;
        humPtr = h;
    }

    void begin(const char *indexHtml)
    {
        startWifi();
        setupRoutes(indexHtml);
        Serial.println("HTTP server started!");
    }

    void startWifi()
    {
        bool CheckWifi = (config.staSsid && strlen(config.staSsid) > 0);

        if (CheckWifi)
        {
            Serial.print("Attempting to connect to: ");
            Serial.println(config.staSsid);

            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            delay(100);

            WiFi.begin(config.staSsid, config.staPass);

            uint32_t now = millis();
            while (WiFi.status() != WL_CONNECTED && (millis() - now) < config.timeout)
            {
                delay(500);
                Serial.print(".");
            }
            Serial.println("");

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print("Connected! IP: ");
                Serial.println(WiFi.localIP());
                return;
            }

            Serial.print("Connection Failed! Status Code: ");
            Serial.println(WiFi.status());
            Serial.println("Starting AP mode as fallback...");
        }

        WiFi.mode(WIFI_AP);
        WiFi.softAP(config.ap_ssid, config.ap_pass);
        Serial.print("AP Started. IP Address: ");
        Serial.println(WiFi.softAPIP());
    }

    void setupRoutes(const char *indexHtml)
    {
        server.on("/", HTTP_GET, [indexHtml](AsyncWebServerRequest *req)
                  { req->send(200, "text/html; charset=utf-8", indexHtml); });

        server.on("/sensor", HTTP_GET, [this](AsyncWebServerRequest *req)
        {
            float currentTemp = (tempPtr != nullptr) ? *tempPtr : 0.0;
            float currentHum = (humPtr != nullptr) ? *humPtr : 0.0;

            String json = "{";
            json += "\"temperature\":" + String(currentTemp, 1) + ",";
            json += "\"humidity\":" + String(currentHum, 1);
            json += "}";

            req->send(200, "application/json", json);
        });

        server.begin();
    }
};