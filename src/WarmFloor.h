#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
//#include <iarduino_RTC.h>
//#include <ESP32Servo.h>
#include <WiFi.h>
//#include <Fonts/FreeSerif9pt7b.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS \
  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// iarduino_RTC watch(RTC_DS1302, 16, 5, 4); // Объявляем объект watch для
// работы с RTC модулем на базе чипа DS1302, указывая выводы Arduino
// подключённые к выводам модуля RST, CLK, DAT.

// Servo myservo;  // create servo object to control a servo

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by
// on-board LED),12-19,21-23,25-27,32-33
int servoPin = 18;  // GPIO pin used to connect the servo control (digital out)
// Possible ADC pins on the ESP32: 0,2,4,12-15,32-39; 34-39 are recommended for
// analog input
int potPin = 34;     // GPIO pin used to connect the potentiometer (analog in)
int ADC_Max = 4096;  // This is the default ADC max value on the ESP32 (12 bit
                     // ADC width); this width can be set (in low-level oode)
                     // from 9-12 bits, for a a range of max values of 512-4096

int val;  // variable to read the value from the analog pin
int pos = 0;
// DynamicJsonDocument doc(4096);

// RST, CLK, DAT.
//  номера портов для светодиодов
const int ledPin4 = 4;

// задаём свойства ШИМ-сигнала
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const char *ssid = "TP-LINK_112900";
const char *password = "";

// webServer
TaskHandle_t TaskWebserver;
WiFiServer wifiServer(80);  // Номер порта для сервера
std::string commandWebServer;

uint8_t heating_map[] = {
    0x18, 0x18, 0x30, 0x38, 0x38, 0x30, 0x30, 0x70, 0x70, 0x70, 0x60, 0x60,
    0x60, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc0, 0xc1, 0xc1, 0x80,
    0xc1, 0x81, 0x80, 0xc1, 0x81, 0x80, 0xc1, 0xc1, 0x80, 0xe0, 0xc1, 0xc0,
    0xe0, 0xe0, 0xc0, 0x70, 0x60, 0xe0, 0x30, 0x70, 0x70, 0x38, 0x30, 0x70,
    0x18, 0x38, 0x30, 0x18, 0x18, 0x30, 0x18, 0x18, 0x30, 0x18, 0x38, 0x30,
    0x38, 0x30, 0x30, 0x30, 0x30, 0x70, 0x70, 0x70, 0x60, 0x60, 0x60, 0xe0,
    0xe0, 0xe0, 0xc0, 0xc1, 0xc1, 0xc0, 0xc1, 0x81, 0x80,
};

int seconds;
int minutes;
int hour;

char secondsChar[10];
char minutesChar[10];
char hourChar[10];

const char *twoDigits();
bool isPump = false;
bool isClient = false;

void clearDisplay();
void isr();
void webServer(void *pvParameters);
void setUnixTime();
void testServo();
void setWatchTime(int subt = 1632953602);
void testRead();
void connectToWiFi();
void setupDisplay();
void checkWifiOn(int updateInterval = 10000);
const char *constructorCommand(std::string commands, std::string data = "");

class WarmFloor  // Main Class
{
 private:
  // список свойств и методов для использования внутри класса

 public:
  // from Oled
  void isConnected(bool isConnected = false);
  void justConnected(WiFiClient client);
  void readScreen(WiFiClient client, bool send);
  void commands(WiFiClient client, std::string commands = "none");
  void heating(bool isHeating = false);  // Boiler should be
  void pumpOf();
  void pumpOn();
  void wifiOn();
  void wifiOf();
  void startStop();
  void setup();
  void clear();
  void show();
  void setText(const char *text, int size = 1, int posX = 0, int posY = 0,
               int width = 15, int height = 15);
  // int timerTestTimer;
  // void testTimer(int timer = 3000) {
  //   if(millis() - timerTestTimer >= timer) {
  //     timerTestTimer = millis();
  //     heating(true);
  //   }
  // }
 protected:
  // список средств, доступных при наследовании
};