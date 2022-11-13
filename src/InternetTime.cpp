#include "InternetTime.h"

uint32_t InternetTime::getUnixTime(String payload) {
  std::string unixtime = payload.c_str();
  std::string unixtimeStr("unixtime");
  std::string utcDatetime("utc_datetime");  // end of search
  std::size_t found = unixtime.find(unixtimeStr);
  // if (found != std::string::npos) { }
  unixtime.erase(0, found - 1);
  found = unixtime.find(utcDatetime);
  unixtime.erase(found - 2, unixtime.size());
  const char *unixtimeChr = unixtime.c_str();
  Serial.println(unixtimeChr);
  const char *unixT = unixtime.erase(0, 11).c_str();
  Serial.println(unixT);
  uint32_t curUnix;  
  std::stringstream ss(unixT);
  int x = 0;
  ss >> x;
  curUnix = x;
  ss >> curUnix;
  Serial.print("unixT= ");
  Serial.println(unixT);
  Serial.print("curUnix ");
  Serial.println(curUnix);
  currentTime = curUnix;
  return curUnix;
}

int counterErrors = 0;
void InternetTime::getInternetTime() {
  String payload;  // Make the request
  boolean isSuccess = false;
 // uint32_t toReturn = 0;
  if ((WiFi.status() == WL_CONNECTED)) {  // Check the current connection status
    HTTPClient http;
    http.begin("http://worldtimeapi.org/api/timezone/Europe/Kiev");  // Specify the URL
    int httpCode = http.GET();
    if (httpCode > 0) {  // Check for the returning code
      payload = http.getString();
      // Serial.println(httpCode);
      Serial.println("payload");
      Serial.println(payload);
      isSuccess = true;
    } else {
      isSuccess = false;
      counterErrors++;
      Serial.println("Error on HTTP request" + isSuccess);
    }
    http.end();  // Free the resources

    if (isSuccess) {
      counterErrors = 0;
      getUnixTime(payload);
      //Serial.println("toReturn " + toReturn);
    } else if (counterErrors < 5) {
      delay(2000);
      getInternetTime();
    }
  }

  //return toReturn;
}

void InternetTime::setTime() {
  getInternetTime();
  uint32_t currentUnixTime = currentTime + 10800 - 3600; // - 3600 for winter time
  Serial.print("currentUnixTime");
  Serial.println(currentUnixTime);
  if (currentUnixTime != 10800 - 3600) {
    setWatchTime(currentUnixTime);
  }
}

void InternetTime::setWatchTime(long int subt) {
    Serial.print("subt ");
  Serial.println(subt);
  uint32_t days = (uint32_t)floor(subt / 86400);
  hour = (uint32_t)floor(((subt - days * 86400) / 3600) % 24);
  minutes = (uint32_t)floor((((subt - days * 86400) - hour * 3600) / 60) % 60);
  seconds = (uint32_t)floor(
      ((((subt - days * 86400) - hour * 3600) - minutes * 60)) % 60);

  Serial.println("Time");
  Serial.println(hour);
  Serial.println(minutes);
  Serial.println(seconds);
}