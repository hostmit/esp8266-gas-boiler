#ifndef RESTART_H
#define RESTART_H

#include "logger.h"
#include "config.h"

class Restart {
    public:

    static bool espRestartRequest;
    static bool espResetRequest;

    static void handle() {
    if (espResetRequest)
    {
        Logger::syslog("espResetRequest request received by loop(), will reset after delay:" + String(ESPRESETDELAY));
        delay(ESPRESETDELAY);
        ESP.reset();
    }
    if (espRestartRequest)
    {
        Logger::syslog("espRestartRequest request received by loop(), will reset after delay:" + String(espRestartRequest));
        delay(ESPRESTARTDELAY);
        ESP.restart();
    }
    }

};


bool Restart::espRestartRequest = false;
bool Restart::espResetRequest= false;

#endif