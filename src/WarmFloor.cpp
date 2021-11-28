#include "Ex.h"
#include "Exter.h"
#include "Screen/Screen.h"
Screen screen;

void WarmFloor::begin() {
  screen.begin();
  clearDisplay();
  connectToWiFi();
  setUnixTime();
}
void setupDisplay() {

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
}

void WarmFloor::wifiOf() {
  BLACK
  g_display.drawBox(118, 0, 10, 8);
  g_display.sendBuffer();
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
}

void WarmFloor::pumpOf() {
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
  if (millis() - timerDisplayTime >= updateInterval) {
    timerDisplayTime = millis();  // сброс таймера
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn);
    uint8_t legnthTime = g_display.getStrWidth("00:00");
    uint8_t center = SCREEN_WIDTH / 2 - legnthTime / 2;
    g_display.drawBox(center, 15, legnthTime, 18);
    g_display.setCursor(center,
                      20);  // Start at top-left corner
    // std::string strin;
    //  strin += hourChar;
    // strin += ":";
    //  strin += minutesChar;
    //  strin += ":";          little space on screen
    //  strin += secondsChar;
    WHITE
    g_display.print(F(twoDigits()));
    g_display.sendBuffer();
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

void WarmFloor::commands(WiFiClient client, std::string commands) {
   Serial.print("Task On Core ");
   Serial.println(xPortGetCoreID());
  if (commands.compare("pumpOn") == 0) {
    warmFloor.pumpOn();
    client.println(constructorCommand("pumpOn"));
  }
  if (commands.compare("pumpOf") == 0) {
    warmFloor.pumpOf();
    client.println(constructorCommand("pumpOf"));
  }
    if (commands.compare("heatingOf") == 0) {
    warmFloor.heating(false);
    client.println(constructorCommand("heatingOf"));
  }
    if (commands.compare("heatingOn") == 0) {
    warmFloor.heating(true);
    client.println(constructorCommand("heatingOn"));
  }
  if (commands.compare("updateTime") == 0) {
    setUnixTime();
    warmFloor.setDispalyTime();
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
    client.println(constructorCommand("pumpOf"));
  }
  warmFloor.readScreen(client, true);
}
void webServer(void *pvParameters) {
  for (;;) {
    WiFiClient client = wifiServer.available();
    if (client) {
      warmFloor.isConnected(true);
      warmFloor.justConnected(client);
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
           // warmFloor.commands(client, commandWebServer);
           wifiClient = client;
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
      warmFloor.isConnected(false);
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
void commandsStart(void *pvParameters){
    warmFloor.commands(wifiClient, wifiCommands);
    vTaskDelete(TaskCommandsCore1);
}
void createCommandsCore1(){
    xTaskCreatePinnedToCore(
      commandsStart,       /* Task function. */
      "TaskWebserver", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      &TaskCommandsCore1,  /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */
}