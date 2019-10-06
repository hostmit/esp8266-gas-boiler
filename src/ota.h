#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "logger.h"
#include "gpioswitch.h"

class Ota
{
  public:


    static unsigned int OtaProgress;
    static unsigned int OtaProgressPrev;
    /*led to blynk on progress as argument actually could be optional*/
    static void init()
    {
        ArduinoOTA.onStart([]() {
            log(F("\n --- OTA update started ---\n"));
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = F("sketch");
            else // U_SPIFFS
                type = F("filesystem");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            OtaProgress = progress / (total / 100);
            if (OtaProgressPrev != OtaProgress && OtaProgress % 10 == 0)
            {
                log("OTA Progress: " + String(OtaProgress));
                OtaProgressPrev = OtaProgress;

                //Gpioswitch::built_in_led.invert();
            }
        });

        ArduinoOTA.onError([](ota_error_t error) {
            log("OTA Error: " + String(error));
            if (error == OTA_AUTH_ERROR)
                log("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                log("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                log("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                log("Receive Failed");
            else if (error == OTA_END_ERROR)
                log("End Failed");
        });

        ArduinoOTA.begin();
    }

    static void handle() {
        ArduinoOTA.handle();
    }

    static void log(String msg) {
        Logger::syslog(msg);

    }
};

unsigned int Ota::OtaProgress;
unsigned int Ota::OtaProgressPrev = 0;

#endif