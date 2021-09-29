#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <ThreeWire.h>  
#include <iarduino_RTC.h>
//#include <ArduinoJson-v6.18.0.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <sstream>
#include <ESP32Servo.h> 

Servo myservo;  // create servo object to control a servo

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by on-board LED),12-19,21-23,25-27,32-33 
int servoPin = 18;      // GPIO pin used to connect the servo control (digital out)
// Possible ADC pins on the ESP32: 0,2,4,12-15,32-39; 34-39 are recommended for analog input
int potPin = 34;        // GPIO pin used to connect the potentiometer (analog in)
int ADC_Max = 4096;     // This is the default ADC max value on the ESP32 (12 bit ADC width);
                        // this width can be set (in low-level oode) from 9-12 bits, for a
                        // a range of max values of 512-4096
  
int val;    // variable to read the value from the analog pin
int pos = 0; 
       // DynamicJsonDocument doc(4096);

//RST, CLK, DAT.
iarduino_RTC watch(RTC_DS1302, 16, 5, 4); // Объявляем объект watch для работы с RTC модулем на базе чипа DS1302, указывая выводы Arduino подключённые к выводам модуля RST, CLK, DAT.

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// номера портов для светодиодов
const int ledPin4 = 4;

// задаём свойства ШИМ-сигнала
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const char *ssid = "TP-LINK_112900";
const char *password = "";

int seconds;
int minutes;
int hour;

char secondsChar[10];
char minutesChar[10];
char hourChar[10];

void setUnixTime();
void testServo();
void setWatchTime();
void testRead();
void connectToWiFi();
void setupDisplay();

void setup()
{
  Serial.begin(9600);
  //delay(4000);
  setupDisplay();
  connectToWiFi();
  
  watch.begin();
  setUnixTime();
  
  display.clearDisplay();
  display.setTextSize(2);                                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);                    // Draw white text
  display.setCursor(0, SCREEN_HEIGHT / 4); // Start at top-left corner
  display.println(F("Loading..."));
  display.display();
  setWatchTime();
  pinMode(17, OUTPUT);
    pinMode(23, OUTPUT);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(19, INPUT_PULLDOWN);
  digitalWrite(17, HIGH);
  digitalWrite(23, HIGH);


//  myservo.setPeriodHertz(50); 
//  myservo.attach(18);
//         myservo.write(180);
//         delay(1000);
//                 myservo.write(0);
// delay(1000);
//         myservo.write(180);

}
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
}
void setupDisplay(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
}
int ts = 0;

int readTimer;
void testRead() {
  if (millis() - readTimer >= 400)
  {
    readTimer = millis();
    if (digitalRead(18) > 0)  {
       digitalWrite(17, HIGH);
      Serial.println(digitalRead(18));

    } else {
      digitalWrite(17, LOW);
    }

    // if (digitalRead(19) > 0)
    // {
    //   Serial.println(digitalRead(19));
    //   //digitalWrite(17, LOW);
    // }
    // if(analogRead(35)) {
    //   Serial.println(analogRead(35));
    // }
  }
}
int timerTestServo;
void testServo () {
  if (millis() - timerTestServo >= 1100) {
    timerTestServo = millis();
    if (ts == 0) {
        myservo.write(0);
        ts = 1;
    } else {
        myservo.write(180);
        ts = 0;
    }
  }
 
}
void setTime() {
//watch.settime(__DATE__, __TIME__);

}
uint32_t timerDisplayTime;
void setDispalyTime(int updateInterval = 1000)
{
  if (millis() - timerDisplayTime >= updateInterval)
  {
    timerDisplayTime = millis(); // сброс таймера
    display.clearDisplay();
    display.setTextSize(2);                                 // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);                    // Draw white text
    display.setCursor(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5); // Start at top-left corner
    //Serial.println(secondsChar);
    std::string strin;
     strin += hourChar;
     strin += ":";
     strin += minutesChar;
     strin += ":";
     strin += secondsChar;
     
    display.println(F(strin.c_str()));
    display.display();
  }
}
void getTime(){}
int duratin(int milis = 3000)
{
  return milis / 255;
}

int del = 5000;
void onn(int del = 5000)
{
  digitalWrite(4, LOW);
  display.clearDisplay();

  display.setTextSize(4);                                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);                    // Draw white text
  display.setCursor(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4); // Start at top-left corner
  display.println(F("ON"));

  //  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  //  display.println(3.141592);
  //
  //  display.setTextSize(2);             // Draw 2X-scale text
  //  display.setTextColor(SSD1306_WHITE);
  //  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(del);
}

void off(int del = 5000)
{
  digitalWrite(4, HIGH);
  display.clearDisplay();
  display.setTextSize(4);                                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);                    // Draw white text
  display.setCursor(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4); // Start at top-left corner
  display.println(F("OF"));

  //  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  //  display.println(3.141592);
  //
  //  display.setTextSize(2);             // Draw 2X-scale text
  //  display.setTextColor(SSD1306_WHITE);
  //  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(del);
}
uint32_t timer1;

void drawCircleUp(int dutyCycle = 0)
{
  if (millis() - timer1 >= 500) {   // таймер на 500 мс (2 раза в сек)
    timer1 = millis();              // сброс таймера
  display.clearDisplay();
  dutyCycle = dutyCycle * 100 / 255 * 30 / 100;

  display.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, dutyCycle, WHITE);
  display.display();
  }


}
void drawCircleDown(int dutyCycle = 255)
{

  if (millis() - timer1 >= 500)
  { // таймер на 500 мс (2 раза в сек)
    timer1 = millis();
    display.clearDisplay();
    dutyCycle = dutyCycle * 100 / 255 * 30 / 100;
    display.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, dutyCycle, WHITE);
    display.display();
  }
}
// uint32_t timer1;
//   if (millis() - myTimer1 >= 500) {   // таймер на 500 мс (2 раза в сек)
//     myTimer1 = millis();              // сброс таймера
//     // выполнить действие 1
//     // 2 раза в секунду
//   }
int dutyCycleUp = 0;
int dutyCycleDown = 255;
void brightnesUp();
void brightnesDown();
void brightnesUp() {
  if (millis() - timer1 >= duratin(3000))
  {
    timer1 = millis();
    // меняем яркость светодиода
    // увеличиваем яркость светодиода
    dutyCycleUp++;
    ledcWrite(ledChannel, dutyCycleUp);
    drawCircleUp(dutyCycleUp);
    if (dutyCycleUp >= 255) {
      brightnesDown();
    }

  }
}
void brightnesDown()
{
  if (millis() - timer1 >= duratin(3000))
  {
    timer1 = millis();
    // уменьшаем яркость светодиода
    dutyCycleDown--;
    ledcWrite(ledChannel, dutyCycleDown);
    drawCircleDown(dutyCycleDown);
    if (dutyCycleDown <= 0)
    {

      brightnesUp();
    }
  }
}
int currentTime;
uint32_t getUnixTime(String payload = "unixtime:1622493860") {
      std::string unixtime = payload.c_str(); 
      std::string unixtimeStr("unixtime");
      std::string utcDatetime("utc_datetime"); // end of search
      std::size_t found = unixtime.find(unixtimeStr);
      if (found != std::string::npos)
      {
      }
      unixtime.erase(0, found - 1);
      found = unixtime.find(utcDatetime);
      unixtime.erase(found - 2, unixtime.size());
      const char *unixtimeChr = unixtime.c_str();
      Serial.println(unixtimeChr);
      const char * unixT = unixtime.erase(0, 11).c_str();
      //DeserializationError err = deserializeJson(doc, unixtimeChr);
      //auto unixT = doc["unixtime"].as<char*>();
      Serial.println(unixT);
      //unixtime.erase(0, 11);
      uint32_t curUnix;//std::strtoul(unixT, nullptr, 0);
      std::stringstream ss(unixT);
      //  ss << unixtime;
      int x = 214;
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
int iii = 0;
uint32_t getInternetTime() {
    String payload;                                        //Make the request
    boolean isSuccess = false;
    uint32_t toReturn = 0;
    if ((WiFi.status() == WL_CONNECTED))
    { //Check the current connection status

      HTTPClient http;

      http.begin("http://worldtimeapi.org/api/timezone/Europe/Helsinki"); //Specify the URL
      int httpCode = http.GET();
      if (httpCode > 0)
      { //Check for the returning code

        payload = http.getString();
        // Serial.println(httpCode);
        Serial.println(payload);
        isSuccess = true;
      }

      else
      {
        isSuccess = false;
        Serial.println("Error on HTTP request");
      }

      http.end(); //Free the resources
    }
    if (isSuccess)
    {
      iii = 0;
      toReturn = getUnixTime(payload);
    }
    else if (iii < 5)
    {
      delay(2000);
      getInternetTime();
    }
    return toReturn;
}

void setUnixTime(){
//watch.settimeUnix(1622501891 + 10800); // + 3600 * 3 need for right  GMT+03:00 
uint32_t currentUnixTime = getInternetTime() + 10800;
//currentUnixTime = currentTime + 10800;
Serial.println(currentUnixTime);
if (currentTime != 10800) {
watch.settimeUnix(currentUnixTime);

}
}

void setWatchTime() {
seconds = watch.seconds;
minutes = watch.minutes;
hour = watch.Hours;
}
uint32_t timerTickTime;
void tickTime() {
  if (millis() - timerTickTime >= 1000){  // update interval
  timerTickTime = millis();
  seconds++;
  if (seconds > 59)
  {
    seconds = 0;
    minutes++;
    if (minutes > 59)
    {
      minutes = 0;
      hour++;
      if (hour > 23)
      {
        hour = 0;
      }
    }
  }
 }
sprintf(secondsChar, "%d", seconds);
sprintf(minutesChar, "%d", minutes);
sprintf(hourChar, "%d", hour);
}
void getSensorTime() {
    //delay(10000);
    if(millis()%1000==0){                                   // Если прошла 1 секунда.
      Serial.println( watch.gettime("d-m-Y, H:i:s, D") );     // Выводим время.
      //setDispalyTime();
      delay(2);                                             // Приостанавливаем скетч на 1 мс, чтоб не выводить время несколько раз за 1мс.
    }
      Serial.println( watch.gettime("d-m-Y, H:i:s, D") );     // Выводим время.

    delay(1000);

}
void loop()
{
 tickTime(); // interval 1000 
 setDispalyTime(); 
 //testRead();
 //testServo();

}

void testfillrect()
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3)
  {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}
void testfillrect2()
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 3; i += 3)
  {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 3, display.height() - i * 3, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}
void testdrawrect()
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2)
  {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}
//void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void fillCircle3()
{
  //display.clearDisplay();
  display.fillRect(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SSD1306_WHITE);
  display.display();
  //  display.drawPixel(11, 11 SSD1306_WHITE);
  //  display.drawPixel(12, 12, SSD1306_WHITE);
  //  display.drawPixel(13, 13, SSD1306_WHITE);
  //  display.drawPixel(14, 14, SSD1306_WHITE);
  //  display.drawPixel(15, 15, SSD1306_WHITE);
}
