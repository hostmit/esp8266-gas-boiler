#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <FS.h>
#include "timetools.h"
#include "config.h"
//#include "gpioswitch.h"

//You need SPIFFS to be ready - SPIFFS.begin();
class Logger
{
public:
  static void syslog(String msg, String fileName);
};

void Logger::syslog(String msg, String fileName = LOGFILENAME)
{
  if (msg.length() > LOGLINEMAXLENGTH)
  {
    msg = "Log Message over limit: " + String(LOGLINEMAXLENGTH) + ". Will be cutted. \n" + msg.substring(0, LOGLINEMAXLENGTH);
  }

  msg = Timetools::formattedTime() + " " + msg;

  File f = SPIFFS.open(fileName, "a+");
  if (!f)
  {
#ifdef LOGUSESERIAL
    Serial.println(fileName + " open failed");
#endif
    return;
  }
  f.println(msg);

  //rotation thing
  if (f.size() > LOGFILESIZE)
  {
    String prevFileName = fileName.substring(0, fileName.lastIndexOf('.')) + "." + LOGPREVEXT;
    f.println("File over LOGFILESIZE:" + String(LOGFILESIZE) + ", will be renamed to " + prevFileName + ", old one removed if exists");
    if (SPIFFS.exists(prevFileName))
      SPIFFS.remove(prevFileName);
    f.close();
    SPIFFS.rename(fileName, prevFileName);
  }
  if (f)
    f.close();
#ifdef LOGUSESERIAL
  Serial.println(msg);
#endif
  return;
}

#endif