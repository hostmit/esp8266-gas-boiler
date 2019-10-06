#ifndef SYSINFO_H
#define SYSINFO_H

#include <Arduino.h>
#include <FS.h>
#include <BlynkSimpleEsp8266.h>
#include "config.h"

class Sysinfo
{
public:
  static long loopCount;
  static long loopStat[LOOPSTATSIZE];
  static long loopLastMillis;

  static long wifiConnectTime;
  static long wifiConnectCount;
  static bool wifiFirstConnected;

  static String getResetReason()
  {
    String t;
    t+= "--- PREVIOUS RESET --- \n";
    t += "getResetInfo(): " + String(ESP.getResetInfo()) + "\n";
    t += "getResetReason(): " + String(ESP.getResetReason()) + "\n";
    rst_info *myResetInfo;
    myResetInfo = ESP.getResetInfoPtr();
    t += "getResetInfoPtr(): " + String(myResetInfo->reason) + "\n";
    t += "\n";
    return t;
  }

  static String getFsInfo()
  {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    String t = "--- FS INFO ---\n";
    t += "totalBytes: " + String(fs_info.totalBytes) + "\n";
    t += "usedBytes: " + String(fs_info.usedBytes) + "\n";
    t += "blockSize: " + String(fs_info.blockSize) + "\n";
    t += "pageSize: " + String(fs_info.pageSize) + "\n";
    t += "maxOpenFiles: " + String(fs_info.maxOpenFiles) + "\n";
    t += "maxPathLength: " + String(fs_info.maxPathLength) + "\n";
    return t;

  }

  static String getRamInfo()
  {
    String t;
    t += F("<strong>--- RAM: getFreeHeap(): </strong>");
    t += String(ESP.getFreeHeap()) + "\n";
    return t;
  }

  static String getFlashInfo()
  {
    String t;
    t += "--- SPI FLASH + CHIP --- \n";
    t += "getFreeSketchSpace(): " + String(ESP.getFreeSketchSpace()) + "\n";
    t += "getSketchSize(): " + String(ESP.getSketchSize()) + "\n";
    t += "getChipId(): " + String(ESP.getChipId()) + "\n";
    t += "getFlashChipMode(): " + String(ESP.getFlashChipMode()) + "\n";
    t += "getFlashChipId(): " + String(ESP.getFlashChipId()) + "\n";
    t += "getFlashChipRealSize(): " + String(ESP.getFlashChipRealSize()) + "\n";
    t += "getFlashChipSize(): " + String(ESP.getFlashChipSize()) + "\n";
    t += "getFlashChipSizeByChipId(): " + String(ESP.getFlashChipSizeByChipId()) + "\n";
    t += "getFlashChipSpeed(): " + String(ESP.getFlashChipSpeed()) + "\n";
    t += "checkFlashConfig(): " + String(ESP.checkFlashConfig()) + "\n";
    t += "getBootMode(): " + String(ESP.getBootMode()) + "\n";
    t += "getBootVersion(): " + String(ESP.getBootVersion()) + "\n";
    t += "getCoreVersion(): " + String(ESP.getCoreVersion()) + "\n";
    t += "getCpuFreqMHz(): " + String(ESP.getCpuFreqMHz()) + "\n";
    t += "getCycleCount(): " + String(ESP.getCycleCount()) + "\n";
    t += "\n";
    return t;
    
  }

  static String getAll()
  {
    return getResetReason() + getFsInfo() + getRamInfo() + getFlashInfo();
  }
  static void loopStatHandle()
  {
    loopCount++;
    if (millis() - loopLastMillis > LOOPSTATDURATION)
    {
      std::copy(loopStat, loopStat + LOOPSTATSIZE - 1, loopStat + 1);
      loopStat[0] = loopCount;
      loopLastMillis = millis();
      loopCount = 0;
    }
  }
  static String getLoopStat()
  {
    String t = "";
    t += "<strong>--- LOOP() STATS</strong>. Duration: " + String(LOOPSTATDURATION / 1000) + " sec each---\n";
    int loopAverage = 0;
    for (int i = 0; i < LOOPSTATSIZE; i++)
    {
      t += String(loopStat[i]);
      if (i != LOOPSTATSIZE - 1)
        t += ",";
      loopAverage += loopStat[i];
    }
    t += "\n";
    t += "Average: " + String(round(loopAverage / LOOPSTATSIZE)) + "\n";
    return t;
  }

  static String getWifi(){
    String t = "";
    t += "wifiConnectTime: " + Timetools::msToFormattedInterval(millis() - wifiConnectTime) + ", ";
            t += "wifiConnectCount: " + String(wifiConnectCount) + "\n";
    t += "SSID: " + WiFi.SSID() + ": Station connected, IP: " + WiFi.localIP().toString() + 
        ", WiFi.subnetMask(): " + WiFi.subnetMask().toString() + ", WiFi.gatewayIP(): " + WiFi.gatewayIP().toString() +
        ", WiFi.dnsIP(): " + WiFi.dnsIP().toString() + "\n";
    return t;
  }
};

long Sysinfo::loopCount = 0;
long Sysinfo::loopLastMillis = 0;
long Sysinfo::loopStat[LOOPSTATSIZE];

long Sysinfo::wifiConnectTime = 0;
long Sysinfo::wifiConnectCount = 0;
bool Sysinfo::wifiFirstConnected = false;
#endif
