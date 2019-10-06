#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <map>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <BlynkSimpleEsp8266.h>

#define SWVERSION 1.0



//logger
#define LOGUSESERIAL true
#define LOGFILENAME "syslog.log"
#define LOGLINEMAXLENGTH 1000
#define LOGFILESIZE 10000
#define LOGPREVEXT "prev"


#define FORMAT_SPIFFS_IF_FAILED true //It will try to format FS if fails to mount

#define NTPSERVER "0.ua.pool.ntp.org"
#define NTPGMTOFFSETSEC 7200
#define NTPDAYLIGHTOFFSETSEC 3600
#define NTPRETRYDELAY 10000
#define NTPOFFSETHOURS 3
#define NTPUDPTIMEOUT 5000
#define NTPUPDATEINTERVAL 3600000



#define ESPRESTARTDELAY 2000
#define ESPRESETDELAY 2000

#define ONBOARDLEDPIN 2

#define LOOPSTATSIZE 10
#define LOOPSTATDURATION 1000


#define HEATEROPTOCOUPLERGPIO 12 // u need a resistor ~ 1 kohm => 2.22 mA. My heater transmits 0.5mA over optocoupler's collector
#define HEATERBUTTONGPIO 14 // non-fixed button to shortcut pin to ground
#define HEATERLEDGPIO 16 //u need a resistor ~100 ohm => 3,78mA
#define HEATERBUTTONDELAY 500
#define HEATERSTATEFILE "heater.json"
#define HEATERSTATEKEY "state"

#define HEATERBLYNKSTATEVPIN  0
#define HEATERBLYNKBUTTONVPIN  1


#define BLYNKBUTTONDELAY 500


#define WEBAPIKEYNAME "apikey"
#define WEBRESPONSEJSONSTATUSKEY "status"

#endif

