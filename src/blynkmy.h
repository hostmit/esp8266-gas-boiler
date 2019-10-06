#ifndef BLYNKMY_H
#define BLYNKMY_H


#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include "logger.h"
#include "config.h"
#include "sysinfo.h"
#include "heater.h"


class BlynkMy
{
  public:
    static long buttonPressedMillis;
        
    static void init()
    {

        Blynk.config(BLYNKTOKEN, BLYNKHOST, BLYNKPORT);
    }

    static void handle()
    {
                
        
        
        Blynk.run();
    }
};

long BlynkMy::buttonPressedMillis = 0;


BLYNK_CONNECTED()
{
    Heater::currentState ? Heater::blynkStateLed.on() : Heater::blynkStateLed.off();
}
BLYNK_WRITE(HEATERBLYNKBUTTONVPIN) {
    if (millis()-BlynkMy::buttonPressedMillis > BLYNKBUTTONDELAY) {
        BlynkMy::buttonPressedMillis = millis();
        Logger::syslog("State change requested from Blynk!");
        Heater::changeState();
    }
}






#endif
