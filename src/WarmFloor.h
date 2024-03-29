#ifndef WarmFloor_H
#define WarmFloor_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <sstream>
#include <string>

#include "Screen/Screen.h"
#include <Preferences.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define WHITE g_display.setColorIndex(1);
#define BLACK g_display.setColorIndex(0);

class WarmFloor  // Main Class
{
 private:
  // список свойств и методов для использования внутри класса

 public:
  // from Oled
  // network
  //bool isNeedPumpStart = false;
 // bool isStillRun = false;
Preferences preferences;

  volatile int howLongPump = 15000;
  int howLongBoiler = 25000;
  volatile bool showCurrentTime = true;
  volatile bool isPumpRunnnig = false; // only for loop
  bool isPump = false; // state of 
  bool isHeating = false; // state of 
  bool isBoilerHeating = false; // only for loop
  volatile bool isWifi = false;
  volatile bool isClient = false;
  void timeLeftDisplay();
  void timeLeftPumpDisplay(const char * timeLeft);
  void timeLeftBoilerDisplay(const char * timeLeft);
  const char * twoDigitsTimeLeft(int timeLeft);
  void timeLeftPump(int howLong = 10000, int updateInterval = 1000);
  void timeLeftBoiler(int howLong = 10000, int updateInterval = 1000);
  void shutdownPump(bool sendScreen = false, bool updateScreen = false);
  void shutdownBoiler(bool sendScreen = false, bool updateScreen = false);
  void startPump(bool sendScreen = false, bool updateScreen = false);
  void startBoiler(bool sendScreen = false, bool updateScreen = false);
  void repairScreen();
  void tickTime();  
  void setDispalyTime(int updateInterval = 1000);
  void begin();
  void taskToCore();
  void isConnected(bool isConnected = false);
  void justConnected(WiFiClient client);
  void readScreen(WiFiClient client, bool sendNow = false);
  void commands(WiFiClient client, const char commands[20]= {0});
  void heating(bool isHeating = false, uint8_t y = 36);  // Boiler should be
  void pumpOff();
  void pumpOn();
  void wifiOn();
  void wifiOf();
  void startStop();
  void setup();
  void clear();
  void show();
  void setText(const char *text, int size = 1, int posX = 0, int posY = 0,
               int width = 15, int height = 15);
  void toCenterDisplay(const char * timeLeft);

 protected:
  // список средств, доступных при наследовании
};

#endif