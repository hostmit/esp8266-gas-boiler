#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <BlynkSimpleEsp8266.h>
#include "config.h"
#include "logger.h"
#include "sysinfo.h"
#include "fstools.h"
#include "timetools.h"
#include "restart.h"
#include "heater.h"

class Web
{
  public:
    static AsyncWebServer server;

    
    static void init()
    {
        root();
        fs();
        sys();
        time();
        api();
        server.begin();
    }

  private:
    static void fs()
    {
        server.on("/fs/list", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();
            String htmlResponse = Fstools::webListRoot();

            request->send(200, "text/html", htmlResponse);
        });
        server.on("/fs/format", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();
            bool result = SPIFFS.format();
            String t = "";
            result == true ? t = "Successfuly formated" : t = "Unable to format";
            request->send(200, "text/plain", t);
        });

        server.on("/fs/read", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();

            if (request->hasArg("name"))
            {
                request->send(SPIFFS, request->arg("name"), "text/plain");
            }
            else
            {
                String htmlResponse = "";
                htmlResponse += "<pre>";
                htmlResponse = +"I need file name";
                htmlResponse += "<a href='/fs/list'>Back to fs list</a>\n";
                htmlResponse += "<a href='/'>Back to /</a>\n";
                htmlResponse += "</pre>";
                request->send(200, "text/html", htmlResponse);
            }
        });

        server.on("/fs/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();
            String htmlResponse = "";
            htmlResponse += "<pre>";
            if (request->hasArg("name"))
            {
                SPIFFS.remove(request->arg("name")) ? htmlResponse += "File removed...\n" : htmlResponse += "Remove file FAILED...\n";
            }
            else
            {
                htmlResponse = +"I need file name";
            }
            htmlResponse += "<hr>";
            htmlResponse += "<a href='/fs/list'>Back to fs list</a>\n";
            htmlResponse += "<a href='/'>Back to /</a>\n";
            htmlResponse += "</pre>";
            request->send(200, "text/html", htmlResponse);
        });
    }

    static void root()
    {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            String t = "";
            t += "<pre>";
            t += "\n";
            t += "/fs/{<a href='/fs/list'>list</a>,<a href='/fs/format' onclick=\"return confirm('Are you sure?')\">format</a>}\n";
            t += "\n";
            t += "<strong>--- Available commands are: --- </strong>\n";
            t += "\n";
            t += "/time/{<a href='/time/update'>update</a>,<a href='/time/resolve'>resolve</a>}\n";
            t += "\n";
            t += "/sys/{<a href='/sys/restart' onclick=\"return confirm('Are you sure?')\">restart</a>,<a href='/sys/reset' onclick=\"return confirm('Are you sure?')\">reset</a>}\n";
            t += "\n";
            t += Sysinfo::getRamInfo();
            t += Sysinfo::getFsInfo();
            t += Sysinfo::getResetReason();
            t += "<strong>--- Current time: </strong>" +  Timetools::formattedTime();
            t += "\n";
            t += "<strong>--- Uptime: </strong>" + Timetools::msToFormattedInterval(millis()) + "\n";
            t += Sysinfo::getLoopStat();
            t += "<strong>--- Timetools: </strong> lastAttempt was " + Timetools::msToFormattedInterval(millis() - Timetools::lastAttempt) + " ago. lastAttempt: " + String(Timetools::lastAttempt) + ", ntpHostResolved: " + String(Timetools::ntpHostResolved) + "\n";
            t += "<strong>--- WiFi: </strong>" + Sysinfo::getWifi();
            t += "<strong>--- blynk.connected(): </strong>" + String(Blynk.connected());
            t += "\n";
            t += "<strong>--- Heater::currentState: </strong>" + String(Heater::currentState);
            t += "\n";
            t += "<strong>SWVERSION: </strong>" + String(SWVERSION);
            t += "</pre>";
            request->send(200, "text/html", t);
        });
    }

    
    static void sys()
    {
        server.on("/sys/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();
            String htmlResponse = "";
            htmlResponse += "<pre>";
            htmlResponse += "ESP.restart() will be issued after delay:" + String(ESPRESTARTDELAY);
            htmlResponse += "<hr>";
            htmlResponse += "<a href='/'>Back to /</a>\n";
            htmlResponse += "</pre>";
            request->send(200, "text/html", htmlResponse);
            Restart::espRestartRequest = true;
        });
        server.on("/sys/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->authenticate(HTTPLOGIN, HTTPPASS))
                return request->requestAuthentication();
            String htmlResponse = "";
            htmlResponse += "<pre>";
            htmlResponse += "ESP.reset() will be issued after delay:" + String(ESPRESTARTDELAY);
            htmlResponse += "<hr>";
            htmlResponse += "<a href='/'>Back to /</a>\n";
            htmlResponse += "</pre>";
            request->send(200, "text/html", htmlResponse);
            Restart::espResetRequest = true;
        });
    }
    static void time()
    {
        server.on("/time/resolve", HTTP_GET, [](AsyncWebServerRequest *request) {
            Timetools::resolveRequested = true;
            request->send(200, "text/html", "ntpServer resolve requested. This can cause delay in loop");
        });
        server.on("/time/update", HTTP_GET, [](AsyncWebServerRequest *request) {
            Timetools::updateRequested = true;
            request->send(200, "text/html", "ntpUpdate requested");
        });
    }
    static void api() {
        server.on("/api/*", HTTP_GET, [](AsyncWebServerRequest *request) {
            Logger::syslog(request->url());
            
            if(strlen(WEBAPIKEY) != 0 && (!request->hasArg(WEBAPIKEYNAME) || request->arg(WEBAPIKEYNAME)!=WEBAPIKEY)) {
                request->send(401);
                return;
            }
            if (request->url()!="/api/boiler/on" && request->url()!="/api/boiler/off" && request->url()!="/api/boiler/invert" && request->url()!="/api/boiler/status") {
                request->send(400);
                return;
            }
            auto respondState = [request]() {
                 StaticJsonDocument<200> doc;
                 doc[WEBRESPONSEJSONSTATUSKEY] = Heater::currentState;
                 String r;
                 serializeJsonPretty(doc,r);
                 request->send(200, "application/json", r);
            };

            if (request->url()=="/api/boiler/on") {
                Heater::changeState(true);
            } else if (request->url()=="/api/boiler/off") {
                Heater::changeState(false);
            } else if (request->url()=="/api/boiler/invert") {
                Heater::changeState(!Heater::currentState);
            } else if (request->url()=="/api/boiler/status") {
            
            }
            respondState();
            
            
        });
    }


};

AsyncWebServer Web::server(80);

#endif