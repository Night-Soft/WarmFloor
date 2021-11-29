#ifndef Exter_h
#define Exter_h
#include <Arduino.h>
#include <string>
#include <WiFi.h>
const char *ssid = "TP-LINK_112900";
const char *password = "";

//create handle for the mutex. It will be used to reference mutex
SemaphoreHandle_t  xMutex;
// webServer
TaskHandle_t TaskWebserver;
TaskHandle_t TaskCommandsCore1;
TaskHandle_t TaskDisconnectCore1;
TaskHandle_t TaskJustConnectedCore1;
WiFiServer wifiServer(80);  // Номер порта для сервера
std::string commandWebServer;
WiFiClient wifiClient;
std::string wifiCommands = "";
#define heating_map_width 24
#define heating_map_height 24
const uint8_t heating_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x1F, 0xDC, 0xF7, 0x3D, 
  0x9C, 0xE7, 0x39, 0xBC, 0xCF, 0x3B, 0x7C, 0xDF, 0x37, 0x3C, 0xCF, 0x33, 
  0x7C, 0xDF, 0x37, 0x3C, 0xCF, 0x33, 0xBC, 0xEF, 0x3B, 0xBC, 0xEF, 0x39, 
  0x9C, 0xE7, 0x39, 0xDC, 0xF7, 0x3D, 0xCC, 0xF3, 0x3C, 0xEC, 0xFB, 0x3E, 
  0xEC, 0xF3, 0x3E, 0xEC, 0xFB, 0x3E, 0xCC, 0xFB, 0x3C, 0xDC, 0xF7, 0x3D, 
  0xDC, 0xE7, 0x39, 0xF8, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  };

uint8_t xDel;
uint8_t yDel;
uint8_t widthDel;

int seconds;
int minutes;
int hour;

char secondsChar[10];
char minutesChar[10];
char hourChar[10];

const char *twoDigits();
void loadingDisplay();
void clearLoadingDisplay(uint8_t x, uint8_t y, uint8_t width);
void commandsStart(void *pvParameters);
void disconnectedStart(void *pvParameters);
void justConnectedStart(void *pvParameters);
void createJustConnectedCore1();
void createCommandsCore1();
void createClientDisconnectedCore1();
void webServer(void *pvParameters);
void clearDisplay();
void isr();
void setUnixTime();
void testServo();
void setWatchTime(int subt = 1632953602);
void testRead();
void connectToWiFi();
void checkWifiOn(int updateInterval = 10000);

const char *constructorCommand(std::string commands, std::string data = "");
#endif