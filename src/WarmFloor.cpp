#include <math.h>
#include <sstream>
#include "Ex.h"
#include "Exter.h"
#include "Screen/Screen.h"
Screen screen;

void WarmFloor::begin() {
  screen.begin();
  clearDisplay();
 // loadingDisplay();
  connectToWiFi();
  setUnixTime();
 // clearLoadingDisplay(xDel,yDel, widthDel);
 xMutex = xSemaphoreCreateMutex();
}
void loadingDisplay() {
  WHITE
  g_display.setFont(u8g_font_unifont);
  uint8_t legnthTime = g_display.getStrWidth("Loading...");
  uint8_t centerWidth = SCREEN_WIDTH / 2 - legnthTime / 2;
  uint8_t centerHight = SCREEN_HEIGHT / 2 - 5;
  g_display.setCursor(centerWidth, centerHight);  // Start at top-left corner
  g_display.print(F("Loading..."));
  g_display.sendBuffer();
  xDel = centerWidth;
  yDel = centerHight;
  widthDel = legnthTime;
}
void clearLoadingDisplay(uint8_t x, uint8_t y, uint8_t width) {
  BLACK
  g_display.drawBox(x-15, y, width + 10, 10);
  g_display.sendBuffer();
}
void WarmFloor::wifiOn() {
  WHITE
  g_display.drawBox(122, 6, 2, 2);

  g_display.drawBox(119, 4, 2, 2);
  g_display.drawBox(125, 4, 2, 2);
  g_display.drawBox(121, 3, 4, 2);

  g_display.drawBox(118, 1, 2, 2);
  g_display.drawBox(126, 1, 2, 2);
  g_display.drawBox(120, 0, 6, 2);
  g_display.sendBuffer();
  warmFloor.isWifi = true;
}

void WarmFloor::wifiOf() {
  BLACK
  g_display.drawBox(118, 0, 10, 8);
  g_display.sendBuffer();
  warmFloor.isWifi = false;
}

void WarmFloor::heating(bool isHeating) {
  // Draw bitmap on the screen
  g_display.setBitmapMode(1);
  uint8_t widthHeight = 24;
  if (isHeating) {
    WHITE
    g_display.drawXBM(0, 20, widthHeight, widthHeight, heating_map);
  } else {
    BLACK
    g_display.drawBox(0, 20, widthHeight, widthHeight);
  }
  g_display.sendBuffer();
  warmFloor.isHeating = isHeating;
}

void WarmFloor::pumpOff() {
  g_display.setFont(u8g_font_unifont);  
  WHITE
  g_display.drawRBox(104, 20, 24, 24, 5);
  BLACK
  g_display.drawRBox(107, 23, 18, 18, 5);
  g_display.setCursor(108, 37);
  WHITE
  g_display.print(F("Of"));
  g_display.sendBuffer();
  warmFloor.isPump = false;
}

void WarmFloor::pumpOn() {
  g_display.setFont(u8g_font_unifont);  
  WHITE
  g_display.drawRBox(104, 20, 24, 24, 5);
  BLACK
  g_display.drawRBox(107, 23, 18, 18, 5);
  g_display.setCursor(108, 37);
  WHITE
  g_display.print(F("On"));
  g_display.sendBuffer();
  warmFloor.isPump = true;
}
int timerHowLongPumpRun;
void WarmFloor::startPump(bool sendScreen, bool updateScreen) {
    timerHowLongPumpRun = millis();
    digitalWrite(4, HIGH);
    isPumpRunnnig = true;
    timeLeftDisplay();
    warmFloor.showCurrentTime = false;
    if (updateScreen) {
      warmFloor.pumpOn();
    }
    if (sendScreen) {
      wifiClient.println(constructorCommand("pumpOn"));
      warmFloor.readScreen(wifiClient, true);
    }

}
int timerHowLongBoilerRun;
void WarmFloor::startBoiler(bool sendScreen, bool updateScreen) {
    timerHowLongBoilerRun = millis();
    digitalWrite(5, HIGH);
    isBoilerHeating = true;
    timeLeftDisplay();
    warmFloor.showCurrentTime = false;
    if (updateScreen) {
      warmFloor.heating(true);
    }
    if (sendScreen) {
      wifiClient.println(constructorCommand("heatingOn"));
      warmFloor.readScreen(wifiClient, true);
    }

}
void WarmFloor::shutdownPump(bool sendScreen, bool updateScreen) {
  digitalWrite(4, LOW);
  isPumpRunnnig = false;
  if (isPumpRunnnig == false && isBoilerHeating == false) {
    warmFloor.showCurrentTime = true;
    repairScreen();
  }
}
void WarmFloor::shutdownBoiler(bool sendScreen, bool updateScreen) {
  digitalWrite(5, LOW);
  isBoilerHeating = false;
  if (isPumpRunnnig == false && isBoilerHeating == false) {
    warmFloor.showCurrentTime = true;
    repairScreen();
  }
}
int timerTimeLeftPump;
void WarmFloor::timeLeftPump(int howLong, int updateInterval) {
  if (millis() - timerHowLongPumpRun >= howLong) {
    timerHowLongPumpRun = millis();
    //warmFloor.shutdownPump(true);
    commands(wifiClient, "pumpOff");
    return;
  }
  if (millis() - timerTimeLeftPump >= updateInterval) {
    timerTimeLeftPump = millis();
    // Here update time left on display
    warmFloor.timeLeftPumpDisplay(twoDigitsTimeLeft(timerHowLongPumpRun - timerTimeLeftPump + howLong));

  }
}
int timerTimeLeftBoiler;
void WarmFloor::timeLeftBoiler(int howLong, int updateInterval) {
  if (millis() - timerHowLongBoilerRun >= howLong) {
    timerHowLongBoilerRun = millis();
    //warmFloor.shutdownBoiler(true);
    commands(wifiClient, "heatingOff");
    return;
  }
  if (millis() - timerTimeLeftBoiler >= updateInterval) {
    timerTimeLeftBoiler = millis();
    // Here update time left on display
    warmFloor.timeLeftBoilerDisplay(twoDigitsTimeLeft(timerHowLongBoilerRun - timerTimeLeftBoiler + howLong));

  }
}
void WarmFloor::timeLeftDisplay() {
  g_display.clearDisplay();
  // pumpOn
  if(warmFloor.isPump){
  g_display.setFont(u8g_font_unifont);  
  WHITE
  g_display.drawRBox(0, 3, 24, 24, 5);
  BLACK
  g_display.drawRBox(3, 6, 18, 18, 5);
  g_display.setCursor(4, 20);
  WHITE
  g_display.print(F("On"));
  }
  // Heating
  if (warmFloor.isHeating){
  g_display.setBitmapMode(1);
  uint8_t widthHeight = 24;
  WHITE
  g_display.drawXBM(0, 36, widthHeight, widthHeight, heating_map);
  }
  // Draw line
  g_display.drawBox(0,32,SCREEN_WIDTH, 2);
  g_display.sendBuffer();
  // wifi / connected
  warmFloor.wifiOn();
  warmFloor.isConnected(true);

}
void WarmFloor::timeLeftPumpDisplay(const char * timeLeft) {
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn); //u8g_font_unifont = 10px
    uint8_t legntghTime = g_display.getStrWidth(timeLeft);
    uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
    uint8_t centerY = (SCREEN_HEIGHT / 2 - 12) / 2; // (128-24-60) / 2 + 24
    uint8_t centerYCursor = (SCREEN_HEIGHT / 2 + 12) / 2; // (128-24-60) / 2 + 24
    g_display.drawBox(centerX, centerY, legntghTime, 18);
    g_display.setCursor(centerX, centerYCursor);  // Start at top-left corner
    WHITE
    g_display.print(F(timeLeft));
    g_display.sendBuffer();
}
void WarmFloor::timeLeftBoilerDisplay(const char * timeLeft) {
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn);
    uint8_t legntghTime = g_display.getStrWidth(timeLeft);
    uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
    uint8_t centerY = (SCREEN_HEIGHT / 2 - 12) / 2 + 32; // (128-24-60) / 2 + 24
    uint8_t centerYCursor = (SCREEN_HEIGHT / 2 + 12) / 2 + 32; // (128-24-60) / 2 + 24
    g_display.drawBox(centerX, centerY, legntghTime, 12); // 12 = height font
    WHITE
    g_display.setCursor(centerX, centerYCursor);  // Start at top-left corner
    g_display.print(F(timeLeft));
    g_display.sendBuffer();
}
const char *WarmFloor::twoDigitsTimeLeft(int timeLeft) {
  //Serial.print("timeLeft mil ");
  //Serial.println(timeLeft);
  double hours = 0, minutes = 0, seconds = 0;
  // get hours
  if (timeLeft / (1000 * 3600) >= 1) {
    hours = floor(timeLeft / (1000 * 3600));
    timeLeft = timeLeft - 3600 * hours * 1000;
  }
  // get minutes
  if (timeLeft / (1000 * 60) >= 1) {
    minutes = floor(timeLeft / (1000 * 60));
    timeLeft = timeLeft - 60 * minutes * 1000;
  }
  // get seconds
  if (timeLeft / 1000 >= 1) {
    seconds = floor(timeLeft / 1000);
    timeLeft = timeLeft - seconds * 1000; // miliseconds dont need
  }
   std::string textHours, textMinutes, textSeconds, text;
   std::ostringstream strs;
   strs << hours;
   textHours = strs.str();
   strs.str("");
   strs << minutes;
   textMinutes = strs.str();
   strs.str("");
   strs << seconds;
   textSeconds = strs.str();
  if (hours < 10) {
    textHours = "0" + textHours + ":";
  } else {
    textHours = textHours;
  }
  if (minutes < 10) {
    textMinutes = "0" + textMinutes + ":";
  } else {
    textMinutes = textMinutes;
  }
  if (seconds < 10) {
    textSeconds = "0" + textSeconds;
  } else {
    textSeconds = textSeconds;
  }
  // clear empty
  if(hours == 0){
    textHours = "";
  }
  if (minutes == 0) {
    textMinutes = "";
  }
  text = textHours + textMinutes + textSeconds;
  Serial.println(text.c_str());
  return text.c_str();
}
void WarmFloor::isConnected(bool isConnected) {
  if (isConnected) {
    WHITE
    g_display.drawBox(106, 1, 10, 1);
    g_display.drawBox(108, 0, 2, 3);
    g_display.drawBox(112, 0, 2, 3);
  } else {
    BLACK
    g_display.drawBox(106, 1, 10, 1);
    g_display.drawBox(108, 0, 2, 3);
    g_display.drawBox(112, 0, 2, 3);
  }
  g_display.sendBuffer();
  isClient = isConnected;
}

void clearDisplay(){
  g_display.clearDisplay();
  g_display.sendBuffer();
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
void WarmFloor::setDispalyTime(int updateInterval) {
  if (showCurrentTime == true) {
  if (millis() - timerDisplayTime >= updateInterval) {
    timerDisplayTime = millis();  // сброс таймера
   // xSemaphoreTake(xMutex, portMAX_DELAY);
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn);
    uint8_t legnthTime = g_display.getStrWidth("00:00");
    uint8_t center = SCREEN_WIDTH / 2 - legnthTime / 2;
    g_display.drawBox(center, 15, legnthTime, 18);
    g_display.setCursor(center, 20);  // Start at top-left corner
    WHITE
    g_display.print(F(twoDigits()));
    g_display.sendBuffer();
   // xSemaphoreGive(xMutex);
  }
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
  // GMT+03:00
  // + 3600 * 3 for summer / - 3600 for winter time
  // watch.settimeUnix(1622501891 + 10800); // Now ds1302 not work 
  uint32_t currentUnixTime =
      getInternetTime() + 10800 - 3600;  
  // currentUnixTime = currentTime + 10800;
  Serial.println(currentUnixTime);
  if (currentTime != 10800) {
    setWatchTime(currentUnixTime);
    // watch.settimeUnix(currentUnixTime); 
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
void WarmFloor::tickTime()  // interval 1000
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
void WarmFloor::repairScreen() {
  clearDisplay();
  if (warmFloor.isPump) {
    warmFloor.pumpOn();
  } else {
    warmFloor.pumpOff();
  }
  if (isHeating) {
    warmFloor.heating(true);
  } else {
    warmFloor.heating(false);
  }
  if (WiFi.status() == WL_CONNECTED) {
    warmFloor.wifiOn();
  } else {
    warmFloor.wifiOf();
  }
  if (isClient) {
    isConnected(true);
  } else {
    isConnected(false);
  }
}
void WarmFloor::commands(WiFiClient client, std::string commands) {
   Serial.print("Task On Core ");
   Serial.println(xPortGetCoreID());
  if (commands.compare("pumpOn") == 0) {
    warmFloor.pumpOn();
    warmFloor.startPump(true);
    client.println(constructorCommand("pumpOn"));
    readScreen(client, true);
    
  }
  if (commands.compare("pumpOff") == 0) {
    warmFloor.pumpOff();
    warmFloor.shutdownPump();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("pumpOff"));
    readScreen(client, true);
    
  }
  if (commands.compare("heatingOn") == 0) {
    warmFloor.heating(true);
    warmFloor.startBoiler(true);
    client.println(constructorCommand("heatingOn"));
    readScreen(client, true);
  }
    if (commands.compare("heatingOff") == 0) {
    warmFloor.heating(false);
    warmFloor.shutdownBoiler();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("heatingOff"));
    readScreen(client, true);
  }
  if (commands.compare("updateTime") == 0) {
    setUnixTime();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("updateTime"));
    readScreen(client, true);
  }
  if (commands.compare("readScreen") == 0) {
    readScreen(client, true);
    client.println(constructorCommand("readScreen"));
  }
  if (commands.compare("clearDisplay") == 0) {
    clearDisplay();
    client.println(constructorCommand("clearDisplay"));
  }
    if (commands.compare("repairScreen") == 0) {
    client.println(constructorCommand("repairScreen"));
    warmFloor.repairScreen();
  }
  if (commands.compare("reboot") == 0) {
    client.println(constructorCommand("reboot"));
    ESP.restart();
  }
}
void WarmFloor::readScreen(WiFiClient client, bool send) {
    std::string pixels;
    Serial.println("start read");
    screen.getPixels(pixels);
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
  if (warmFloor.isPump) {
    client.println(constructorCommand("pumpOn"));
  } else {
    client.println(constructorCommand("pumpOff"));
  }
  warmFloor.readScreen(client, true);
}
void webServer(void *pvParameters) {
  for (;;) {
    WiFiClient client = wifiServer.available();
    if (client) {
      wifiClient = client;
      createJustConnectedCore1();
    //  xSemaphoreTake(xMutex, portMAX_DELAY);
    //  xSemaphoreGive(xMutex);
      while (client.connected()) {
        while (client.available() > 0) {
          char c = client.read();
          commandWebServer.push_back(c);
          if (commandWebServer.back() == '\n') {  // если байт является переводом строки
            Serial.println("yes last contain");
            commandWebServer.erase(
                commandWebServer.end() - 2,
                commandWebServer.end());  // for correct compare
           // warmFloor.commands(client, commandWebServer);
           wifiCommands = commandWebServer;
           createCommandsCore1();
          Serial.println("Web server on core ");
           Serial.println(xPortGetCoreID());
            commandWebServer = "";
          }
          //Serial.write(c);
        }
        delay(10);
      }

      client.stop();
      Serial.println("Client disconnected");
      createClientDisconnectedCore1();
    }
  }
}
void WarmFloor::taskToCore(){
    // create a task that will be executed in the Task1code() function, with
  // priority 1 and executed on core 0
  wifiServer.begin();
  xTaskCreatePinnedToCore(
      webServer,       /* Task function. */
      "TaskWebserver", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      0,               /* priority of the task */
      &TaskWebserver,  /* Task handle to keep track of created task */
      0);              /* pin task to core 0 */
  delay(100);
}
void commandsStart(void *pvParameters) {
  Serial.println("Commands on core ");
  Serial.println(xPortGetCoreID());
  warmFloor.commands(wifiClient, wifiCommands);
  vTaskDelete(TaskCommandsCore1);
}
void createCommandsCore1(){
    xTaskCreatePinnedToCore(
      commandsStart,       /* Task function. */
      "TaskCommandsCore1", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      2,               /* priority of the task */
      &TaskCommandsCore1,  /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */
}
void disconnectedStart(void *pvParameters) {
  Serial.println("Disconnected on core ");
  Serial.println(xPortGetCoreID());
  warmFloor.isConnected(false);
  vTaskDelete(TaskDisconnectCore1);
}
void createClientDisconnectedCore1(){
    xTaskCreatePinnedToCore(
      disconnectedStart,       /* Task function. */
      "TaskDisconnectCore1", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      &TaskDisconnectCore1,  /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */
}
void justConnectedStart(void *pvParameters) {
      warmFloor.isConnected(true);
      warmFloor.justConnected(wifiClient);
      vTaskDelete(TaskJustConnectedCore1);
}
void createJustConnectedCore1(){
    xTaskCreatePinnedToCore(
      justConnectedStart,       /* Task function. */
      "TaskJustConnectedCore1", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      3,               /* priority of the task */
      &TaskJustConnectedCore1,  /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */
}