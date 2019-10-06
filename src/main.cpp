#include <Arduino.h>
#include <FS.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "config.h"
#include "logger.h"
#include "credentials.h"
#include "ota.h"
#include "timetools.h"
#include "gpioswitch.h"
#include "sysinfo.h"
#include "web.h"
#include "fstools.h"
#include "blynkmy.h"
#include "heater.h"


void setup()
{

    Serial.begin(115200);
    delay(5000);
    Serial.println("Starting...");
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS.begin() failed, will try to format...");
        if (SPIFFS.format())
        {
            Serial.println("SPIFFS.format() success...");
            if (!SPIFFS.begin())
            {
                Serial.println("SPIFFS.begin() failed...");
            }
            else
            {
                Serial.println("SPIFFS.begin() success...");
            }
        }
        else
            Serial.println("SPIFFS.format() fail...");
    }
    else
    {
        Serial.println("SPIFFS.begin() success...");
    }

    Logger::syslog("Setup Started");
    
    WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;
    gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
        Sysinfo::wifiConnectTime = millis();
        Sysinfo::wifiConnectCount++;
        Sysinfo::wifiFirstConnected = true;
        Logger::syslog("Station connected, IP: " + WiFi.localIP().toString() + ", wifiConnectCount: " + String(Sysinfo::wifiConnectCount));
    });

    disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
        Logger::syslog("Station disconnected. wifiConnectTime: " + Timetools::msToFormattedInterval(millis() - Sysinfo::wifiConnectTime));
    });
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFISSID, WIFIPASS);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Logger::syslog("WiFi Connect Failed! Will try again...");
        delay(1000);
    }
    Logger::syslog("Station connected, IP: " + WiFi.localIP().toString() + ", wifiConnectCount: " + String(Sysinfo::wifiConnectCount));
    Web::init();
    
    Ota::init();
    Gpioswitch::built_in_led.start(-1, 50);
    Timetools::init();
    Heater::init();       
    BlynkMy::init();
}

void loop()
{
    Restart::handle();
    Ota::handle();
    Gpioswitch::handle();
    Sysinfo::loopStatHandle();
    Timetools::handle();
    Heater::handle();
    BlynkMy::handle();
    

}