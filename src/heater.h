#ifndef HEATER_H
#define HEATER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "gpioswitch.h"
#include <BlynkSimpleEsp8266.h>


class Heater
{

  public:
    static bool currentState;
    static long buttonPressTime;
    static Gpioswitch led;
    static bool currentButtonValue;

    static WidgetLED blynkStateLed;
    
    static void init()
    {
        pinMode(HEATEROPTOCOUPLERGPIO, OUTPUT);
        pinMode(HEATERBUTTONGPIO, INPUT_PULLUP);
        pinMode(HEATERLEDGPIO, OUTPUT);
        loadState();
        digitalWrite(HEATEROPTOCOUPLERGPIO,currentState);
    }


    static void changeState(bool desiredState = !currentState, String src = "unknown") {
        
        Logger::syslog("heaterState: " + String(currentState) + ", desired state: " + String(desiredState) + ", src: " + src);
        digitalWrite(HEATEROPTOCOUPLERGPIO, desiredState);
        digitalWrite(HEATERLEDGPIO, desiredState);
        desiredState ? led.on() : led.off();
        desiredState ? blynkStateLed.on() : blynkStateLed.off();
        currentState = desiredState;
        saveState();
    }


    static void handle()
    {
        
        if (millis() - buttonPressTime > HEATERBUTTONDELAY && digitalRead(HEATERBUTTONGPIO)==0)
        {
            buttonPressTime = millis();
            changeState();
            Logger::syslog("State change requested via physical button");
        }
    }

    static void saveState()
    {
        File file = SPIFFS.open(HEATERSTATEFILE, "w");
        if (!file)
        {
            Logger::syslog("Failed to create file:" + String(HEATERSTATEFILE));
            return;
        }

        StaticJsonDocument<200> doc;
        doc[HEATERSTATEKEY] = currentState;
        serializeJson(doc, file);
        file.close();
    }

    static void loadState()
    {
        Logger::syslog("Loading Heater State...");

        File file = SPIFFS.open(HEATERSTATEFILE, "r");
        if (!file)
        {
            Logger::syslog("Failed to read file:" + String(HEATERSTATEFILE));
            return;
        }
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Logger::syslog("Failed to parse JSON.");
            return;
        }
        Logger::syslog("State loaded as :" + String(doc[HEATERSTATEKEY].as<String>()));
        changeState(doc[HEATERSTATEKEY]);
        

        file.close();
    }
};

Gpioswitch Heater::led(HEATERLEDGPIO);
bool Heater::currentState = false;
bool Heater::currentButtonValue = false;
long Heater::buttonPressTime = 0;

WidgetLED Heater::blynkStateLed(HEATERBLYNKSTATEVPIN);
#endif