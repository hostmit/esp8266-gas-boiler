#ifndef FSTOOLS_H
#define FSTOOLS_H

#include <Arduino.h>
#include <FS.h>

class Fstools
{
  public:
    static String errMsg;
    static String webListRoot()
    {
        String str = F("<strong>--- Root Directory: </strong><br>");
        str += "<pre>";
        Dir dir = SPIFFS.openDir("");
        while (dir.next())
        {
            str += dir.fileName();
            str += " ---> size: ";
            str += dir.fileSize();
            str += " bytes, --->";
            str += " <a href='/fs/read?name=";
            str += dir.fileName();
            str += "'>read</a>";
            str += " ---> <a href='/fs/delete?name=";
            str += dir.fileName();
            str += "' onclick=\"return confirm('Are you sure?')\">delete</a>";
            str += "\r\n";
        }
        str += "</pre>";
        return str;
    }
    static String fileToString(String fileName)
    {
        String result = "";
        File file = SPIFFS.open(fileName, "r");
        if (!file)
            return "I was unable to read file: " + fileName;
        while (file.available())
        {
            result += char(file.read());
        }
        file.close();
        return result;
    }
};

String Fstools::errMsg = "";

#endif