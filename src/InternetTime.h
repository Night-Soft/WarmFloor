#ifndef InternetTime_H
#define InternetTime_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <sstream>
#include <string>


class InternetTime {
 public:
  void setTime();
  void setWatchTime(long int subt = 1632953602);
  int hour = 0;
  int minutes = 0;
  int seconds = 0;
  

 private:
  int currentTime = 0;
  int counterErrors = 0;
  uint32_t getUnixTime(String payload = "unixtime:1622493860");
  void getInternetTime();
};

#endif