#include <Arduino.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <WarmFloor.h>
#include <Wire.h>

#include <sstream>
#include <string>

WarmFloor warmFloor;

void setup() {
  Serial.begin(9600);

  // delay(4000);
  setupDisplay();
  connectToWiFi();

  // watch.begin();
  setUnixTime();
  pinMode(18, INPUT_PULLDOWN);
  pinMode(19, OUTPUT);
  pinMode(17, OUTPUT);
  digitalWrite(19, HIGH);

  warmFloor.heating(true);
  if (isPump) {
    warmFloor.pumpOn();
  } else {
    warmFloor.pumpOf();
  }
  wifiServer.begin();

  // create a task that will be executed in the Task1code() function, with
  // priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      webServer,       /* Task function. */
      "TaskWebserver", /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      0,               /* priority of the task */
      &TaskWebserver,  /* Task handle to keep track of created task */
      0);              /* pin task to core 0 */
  delay(100);
}
void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
}
void WarmFloor::wifiOn() {
  display.fillRect(122, 6, 2, 2, WHITE);

  display.fillRect(119, 4, 2, 2, WHITE);
  display.fillRect(125, 4, 2, 2, WHITE);
  display.fillRect(121, 3, 4, 2, WHITE);

  display.fillRect(118, 1, 2, 2, WHITE);
  display.fillRect(126, 1, 2, 2, WHITE);
  display.fillRect(120, 0, 6, 2, WHITE);
  display.display();
}

void WarmFloor::wifiOf() {
  display.fillRect(118, 0, 10, 8, BLACK);
  display.display();
}

void WarmFloor::heating(bool isHeating) {
  // Draw bitmap on the screen
  if (isHeating) {
    display.drawBitmap(0, 19, heating_map, 20, 27, WHITE);
  } else {
    display.drawBitmap(0, 19, heating_map, 20, 27, BLACK);
  }
  display.display();
}

void WarmFloor::pumpOf() {
  display.fillRoundRect(104, 20, 24, 24, 5, WHITE);
  display.fillRoundRect(107, 23, 18, 18, 5, BLACK);
  display.setCursor(111, 28);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F("OF"));
  display.display();
  isPump = false;
}

void WarmFloor::pumpOn() {
  display.fillRoundRect(104, 20, 24, 24, 5, WHITE);
  display.fillRoundRect(107, 23, 18, 18, 5, BLACK);
  display.setCursor(111, 28);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F("ON"));
  display.display();
  isPump = true;
}
void WarmFloor::isConnected(bool isConnected) {
  if (isConnected) {
    display.fillRect(106, 1, 10, 1, WHITE);
    display.fillRect(108, 0, 2, 3, WHITE);
    display.fillRect(112, 0, 2, 3, WHITE);
  } else {
    display.fillRect(106, 1, 10, 1, BLACK);
    display.fillRect(108, 0, 2, 3, BLACK);
    display.fillRect(112, 0, 2, 3, BLACK);
  }
  display.display();
}

void clearDisplay(){
  bool pixelColor;
  std::string pixels;
  for (int i = 0; i < 64; i++) {     // this Y
    for (int j = 0; j < 128; j++) {  // this X
      pixelColor = display.getPixel(j, i);
      if (pixelColor) {
        pixels.append("1");
      } else {
        pixels.append("0");
      }
    }
  }
  display.clearDisplay();
  for (int i = 0; i < 64; i++) {     // this Y
    for (int j = 0; j < 128; j++) {  // this X
      display.writePixel(j, i, pixels[i + j]);
    }
  }
  display.display();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    Serial.println("Connecting to WiFi..");
    i++;
    if (i >= 5) {
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to the WiFi network");
    warmFloor.wifiOn();
  } else {
    warmFloor.wifiOf();
  }
}

uint32_t timerCheckWifiOn;
void checkWifiOn(int updateInterval) {
  if (millis() - timerCheckWifiOn >= updateInterval) {
    timerCheckWifiOn = millis();
    if (WiFi.status() != WL_CONNECTED) {
      // WiFi.disconnect(true);
      // WiFi.mode(WIFI_OFF);
      warmFloor.wifiOf();
    }
  }
}

int ts = 0;

int timerReadInput;
void readInput() {
  if (millis() - timerReadInput >= 400) {
    timerReadInput = millis();
    if (digitalRead(18) > 0) {
      digitalWrite(17, HIGH);
      Serial.println(digitalRead(18));

    } else {
      digitalWrite(17, LOW);
    }
  }
}
uint32_t timerDisplayTime;
void setDispalyTime(int updateInterval = 1000) {
  if (millis() - timerDisplayTime >= updateInterval) {
    timerDisplayTime = millis();  // сброс таймера
    // display.clearDisplay(); //
    display.fillRect(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 5, 60, 18, BLACK);
    display.setTextSize(2);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.setCursor(SCREEN_WIDTH / 4,
                      SCREEN_HEIGHT / 5);  // Start at top-left corner
    // Serial.println(secondsChar);
    // std::string strin;
    //  strin += hourChar;
    // strin += ":";
    //  strin += minutesChar;
    //  strin += ":";          little space on screen
    //  strin += secondsChar;

    display.println(F(twoDigits()));
    display.display();
  }
}
const char *twoDigits() {
  std::string textHours, textMinutes, text;
  std::stringstream ssHours(hourChar);  // cast to int
  std::stringstream ssMinutes(minutesChar);
  int iHours, iMinutes;
  ssHours >>
      iHours;  // Now the variable "iHours" holds the value ssHours(hourChar);
  ssMinutes >> iMinutes;
  ssHours << iHours;
  ssMinutes << iMinutes;
  textHours = ssHours.str();
  textMinutes = ssMinutes.str();
  if (iHours < 10) {
    textHours = "0" + textHours;
  } else {
    textHours = textHours;
  }
  if (iMinutes < 10) {
    textMinutes = "0" + textMinutes;
  } else {
    textMinutes = textMinutes;
  }
  text = textHours + ":" + textMinutes;
  return text.c_str();
}
int duratin(int milis = 3000) { return milis / 255; }
int currentTime;
uint32_t getUnixTime(String payload = "unixtime:1622493860") {
  std::string unixtime = payload.c_str();
  std::string unixtimeStr("unixtime");
  std::string utcDatetime("utc_datetime");  // end of search
  std::size_t found = unixtime.find(unixtimeStr);
  if (found != std::string::npos) {
  }
  unixtime.erase(0, found - 1);
  found = unixtime.find(utcDatetime);
  unixtime.erase(found - 2, unixtime.size());
  const char *unixtimeChr = unixtime.c_str();
  Serial.println(unixtimeChr);
  const char *unixT = unixtime.erase(0, 11).c_str();
  // DeserializationError err = deserializeJson(doc, unixtimeChr);
  // auto unixT = doc["unixtime"].as<char*>();
  Serial.println(unixT);
  // unixtime.erase(0, 11);
  uint32_t curUnix;  // std::strtoul(unixT, nullptr, 0);
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
int counterErrors = 0;
uint32_t getInternetTime() {
  String payload;  // Make the request
  boolean isSuccess = false;
  uint32_t toReturn = 0;
  if ((WiFi.status() == WL_CONNECTED)) {  // Check the current connection status
    HTTPClient http;
    http.begin(
        "http://worldtimeapi.org/api/timezone/Europe/Helsinki");  // Specify the
                                                                  // URL
    int httpCode = http.GET();
    if (httpCode > 0) {  // Check for the returning code
      payload = http.getString();
      // Serial.println(httpCode);
      Serial.println(payload);
      isSuccess = true;
    } else {
      isSuccess = false;
      counterErrors++;
      Serial.println("Error on HTTP request");
    }

    http.end();  // Free the resources

    if (isSuccess) {
      counterErrors = 0;
      toReturn = getUnixTime(payload);
    } else if (counterErrors < 5) {
      delay(2000);
      getInternetTime();
    }
  }

  return toReturn;
}

void setUnixTime() {
  // watch.settimeUnix(1622501891 + 10800); // + 3600 * 3 need for right
  // GMT+03:00
  uint32_t currentUnixTime =
      getInternetTime() + 10800 - 3600;  // - 3600 because winter time
  // currentUnixTime = currentTime + 10800;
  Serial.println(currentUnixTime);
  if (currentTime != 10800) {
    setWatchTime(currentUnixTime);
    // watch.settimeUnix(currentUnixTime); Now ds1302 not work
  }
}

void setWatchTime(int subt) {
  // seconds = watch.seconds; now ds1302 not work
  // minutes = watch.minutes;
  // hour = watch.Hours;
  // int subt = 1632953602;
  delay(2000);
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

uint32_t timerTickTime;
void tickTime()  // interval 1000
{
  if (millis() - timerTickTime >= 1000) {  // update interval
    timerTickTime = millis();
    seconds++;
    if (seconds > 59) {
      seconds = 0;
      minutes++;
      if (minutes > 59) {
        minutes = 0;
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
    }
  }
  sprintf(secondsChar, "%d", seconds);
  sprintf(minutesChar, "%d", minutes);
  sprintf(hourChar, "%d", hour);
}

const char *constructorCommand(std::string command, std::string data) {
  if (data.length() >= 1) {
    command.append(" 1 ");  // separation if is command
    command.append(data);
  }
  command.insert(0, " ");  // separation of size
  unsigned short int length = command.length();
  std::string strLength;
  std::stringstream ss;
  ss << length;
  ss >> strLength;
  ss.clear();
  unsigned short int lengthLenth = strLength.length();
  length = length + lengthLenth;
  ss << length;
  ss >> strLength;
  command.insert(0, strLength);
  return command.c_str();
}

void WarmFloor::commands(WiFiClient client, std::string commands) {
  if (commands.compare("pumpOn") == 0) {
    warmFloor.pumpOn();
    client.println(constructorCommand("pumpOn"));
  }
  if (commands.compare("pumpOf") == 0) {
    warmFloor.pumpOf();
    client.println(constructorCommand("pumpOf"));
  }
  if (commands.compare("updateTime") == 0) {
    setUnixTime();
    setDispalyTime();
    client.println(constructorCommand("updateTime"));
  }
  if (commands.compare("readScreen") == 0) {
    readScreen(client, true);
    client.println(constructorCommand("readScreen"));
  }
  if (commands.compare("clearDisplay") == 0) {
    clearDisplay();
    client.println(constructorCommand("clearDisplay"));
  }
}
void WarmFloor::readScreen(WiFiClient client, bool send) {
  bool pixelColor;
  std::string pixels;
  Serial.println("start read");
  for (int i = 0; i < 64; i++) {     // this Y
    for (int j = 0; j < 128; j++) {  // this X
      pixelColor = display.getPixel(j, i);
      if (pixelColor) {
        pixels.append("1");
      } else {
        pixels.append("0");
      }
    }
  }
  Serial.println("end read");
  // for send
  if (send) {
    client.println(constructorCommand("screen", pixels));
    Serial.println("send size = " + pixels.size());
  } else {
    Serial.println("Not send just read");
  }
}
void WarmFloor::justConnected(WiFiClient client) {
  if (isPump) {
    client.println(constructorCommand("pumpOn"));
  } else {
    client.println(constructorCommand("pumpOf"));
  }
  warmFloor.readScreen(client, true);
}
void webServer(void *pvParameters) {
  for (;;) {
    WiFiClient client = wifiServer.available();
    if (client) {
      warmFloor.justConnected(client);
      warmFloor.isConnected(true);
      while (client.connected()) {
        while (client.available() > 0) {
          char c = client.read();
          commandWebServer.push_back(c);
          if (commandWebServer.back() ==
              '\n') {  // если байт является переводом строки
            Serial.println("yes last contain");
            commandWebServer.erase(
                commandWebServer.end() - 2,
                commandWebServer.end());  // for correct compare
            warmFloor.commands(client, commandWebServer);
            commandWebServer = "";
          }
          Serial.write(c);
        }
        delay(10);
      }

      client.stop();
      Serial.println("Client disconnected");
      warmFloor.isConnected(false);
    }
  }
}
void loop() {
  tickTime();  // interval 1000
  setDispalyTime();

  // warmFloor.webServer();
  // checkWifiOn();
  // readInput();
  // testServo();
  // warmFloor.heating(true);
  // delay(4000);
  // warmFloor.heating(false);
  // delay(4000);
}
