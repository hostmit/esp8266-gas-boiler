#ifndef TIMETOOLS_H
#define TIMETOOLS_H

#include <time.h>
#include <Arduino.h>
#include "config.h"
#include <WiFiUdp.h>

class Timetools
{
  public:
    static WiFiUDP UDP;
    static IPAddress timeServerIP; // time.nist.gov NTP server address
    static bool udpStarted;
    static bool ntpHostResolved;
    static unsigned long lastAttempt;
    static bool working;
    static unsigned long lastNTPMillis;
    static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
    static byte NTPBuffer[NTP_PACKET_SIZE];
    static uint32_t timeUnix;
    static bool resolveRequested;
    static bool updateRequested;

    static void init()
    {
        if (WiFi.isConnected())
        {
            ntpHostResolved = resolveNtpHost();
            udpStarted = UDP.begin(123); // Start listening for UDP messages on port 123
            if (udpStarted)
            {
                if (ntpHostResolved)
                {
                    sendNTPpacket();
                    working = true;
                    lastAttempt = millis();
                }
            }
        }
    }
    static void sendNTPpacket()
    {
        memset(NTPBuffer, 0, NTP_PACKET_SIZE); // set all bytes in the buffer to 0
        // Initialize values needed to form NTP request
        NTPBuffer[0] = 0b11100011; // LI, Version, Mode
        // send a packet requesting a timestamp:
        UDP.beginPacket(timeServerIP, 123); // NTP requests are to port 123
        UDP.write(NTPBuffer, NTP_PACKET_SIZE);
        UDP.endPacket();
    }

    static uint32_t parseNtpPacket()
    {
        if (UDP.parsePacket() == 0)
        { // If there's no response (yet)
            return 0;
        }
        UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
        // Combine the 4 timestamp bytes into one 32-bit number
        uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
        // Convert NTP time to a UNIX timestamp:
        // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
        const uint32_t seventyYears = 2208988800UL;
        // subtract seventy years:
        uint32_t UNIXTime = NTPTime - seventyYears + NTPOFFSETHOURS * 60 * 60;
        return UNIXTime;
    }

    static bool resolveNtpHost()
    {
        if (WiFi.hostByName(NTPSERVER, timeServerIP))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static inline int getSeconds(uint32_t &UNIXTime)
    {
        return UNIXTime % 60;
    }

    static inline int getMinutes(uint32_t &UNIXTime)
    {
        return UNIXTime / 60 % 60;
    }

    static inline int getHours(uint32_t &UNIXTime)
    {
        return UNIXTime / 3600 % 24;
    }

    static String getTimeStr()
    {
        String r;
        char buff[3];
        if (timeUnix == 0)
            return "";
        uint32_t actualTime = timeUnix + (millis() - lastNTPMillis) / 1000;
        snprintf(buff, 3, "%02d", getHours(actualTime));
        r += String(buff) + ":";
        snprintf(buff, 3, "%02d", getMinutes(actualTime));
        r += String(buff) + ":";
        snprintf(buff, 3, "%02d", getSeconds(actualTime));
        r += String(buff);
        return r;
    }
    static String formattedTime()
    {
        return "[" + getTimeStr() + "]";
    }

    static void handle()
    {
        if (resolveRequested)
        {
            resolveRequested = false;
            resolveNtpHost();
        }

        if (!ntpHostResolved)
            return;

        //look for udp response
        if (working && millis() - lastAttempt < NTPUDPTIMEOUT)
        {
            uint32_t result = parseNtpPacket();
            if (result != 0)
            {
                timeUnix = result;
                working = false;
                lastNTPMillis = millis();
            }
        }
        //timedout on response
        else if (working && millis() - lastAttempt > NTPUDPTIMEOUT)
        {
            working = false;
        }

        if (!working && (millis() - lastAttempt > NTPUPDATEINTERVAL || updateRequested))
        {
            lastAttempt = millis();
            updateRequested = false;
            if (!udpStarted)
            {
                udpStarted = UDP.begin(123);
            }
            if (udpStarted)
            {
                sendNTPpacket();
                working = true;
            }
        }
    }

    static String msToFormattedInterval(long timeArg = 0)
    {
        String text = "";

        long days = 0;
        long hours = 0;
        long mins = 0;
        long secs = 0;
        timeArg == 0 ? secs = millis() / 1000 : secs = timeArg / 1000;

        //secs = millis() / 1000;      //convect milliseconds to seconds
        mins = secs / 60;            //convert seconds to minutes
        hours = mins / 60;           //convert minutes to hours
        days = hours / 24;           //convert hours to days
        secs = secs - (mins * 60);   //subtract the coverted seconds to minutes in order to display 59 secs max
        mins = mins - (hours * 60);  //subtract the coverted minutes to hours in order to display 59 minutes max
        hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max
        // //Display results
        // text += "--- Running Time ---\n";
        if (days > 0) // days will displayed only if value is greater than zero
        {
            text += String(days) + " days and ";
        }
        if (hours < 10)
            text += "0";
        text += String(hours) + ":";
        if (mins < 10)
            text += "0";
        text += String(mins) + ":";
        if (secs < 10)
            text += "0";
        text += String(secs);
        return text;
    }
};

WiFiUDP Timetools::UDP;
IPAddress Timetools::timeServerIP; // time.nist.gov NTP server address
byte Timetools::NTPBuffer[NTP_PACKET_SIZE];
unsigned long Timetools::lastAttempt = 0;
bool Timetools::working = false;
unsigned long Timetools::lastNTPMillis = 0;
uint32_t Timetools::timeUnix = 0;
bool Timetools::udpStarted = false;
bool Timetools::ntpHostResolved;
bool Timetools::resolveRequested = false;
bool Timetools::updateRequested = false;
#endif