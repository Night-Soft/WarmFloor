#include <math.h>
#include <sstream>
#include "Ex.h"
#include "Exter.h"
#include "Screen/Screen.h"
#include "InternetTime.h"

Screen screen;
InternetTime internetTime;
const char *constructorCommand(std::string commands, std::string data = "");

void WarmFloor::begin() {
  screen.begin();
  loadingDisplay();

  connectToWiFi();
  Serial.println(WiFi.localIP());
  internetTime.setTime();
  hour = internetTime.hour;
  minutes = internetTime.minutes;
  seconds = internetTime.minutes;
  warmFloor.preferences.begin("timeSettings", true);
  warmFloor.howLongPump = warmFloor.preferences.getInt("howLongPump", 15000); 
  warmFloor.howLongBoiler = warmFloor.preferences.getInt("howLongBoiler", 25000);
  
  clearLoadingDisplay(xDel,yDel, widthDel);
  warmFloor.taskToCore(); // start web server

 //xMutex = xSemaphoreCreateMutex();
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
  yDel = centerHight-12;
  widthDel = legnthTime;

}
void clearLoadingDisplay(uint8_t x, uint8_t y, uint8_t width) {
  BLACK
  g_display.drawBox(x, y, width , 12);
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
 // g_display.sendBuffer();
  warmFloor.isWifi = true;
}

void WarmFloor::wifiOf() {
  BLACK
  g_display.drawBox(118, 0, 10, 8);
  warmFloor.isWifi = false;
}

void WarmFloor::heating(bool isHeating, uint8_t y) {
  // Draw bitmap on the screen
  g_display.setBitmapMode(1);
  uint8_t widthHeight = 24;
  if (isHeating) {
    WHITE
    g_display.drawXBM(0, y, widthHeight, widthHeight, heating_map);
  } else {
    BLACK
    g_display.drawBox(0, y, widthHeight, widthHeight);
  }
  warmFloor.isHeating = isHeating;
}

void WarmFloor::pumpOff() {
  g_display.setFont(u8g_font_unifont);  
  WHITE
  g_display.drawRBox(0, 3, 24, 24, 5);
  BLACK
  g_display.drawRBox(3, 6, 18, 18, 5);
  g_display.setCursor(4, 20);
  WHITE
  g_display.print(F("Of"));
  g_display.sendBuffer();
  warmFloor.isPump = false;
}

void WarmFloor::pumpOn() {
  g_display.setFont(u8g_font_unifont);
  WHITE
  g_display.drawRBox(0, 3, 24, 24, 5);
  BLACK
  g_display.drawRBox(3, 6, 18, 18, 5);
  g_display.setCursor(4, 20);
  WHITE
  g_display.print(F("On"));
  g_display.sendBuffer();
  warmFloor.isPump = true;
}
volatile int timerHowLongPumpRun;
void WarmFloor::startPump(bool sendScreen, bool updateScreen) {
    timerHowLongPumpRun = millis();
    digitalWrite(G_PUMP, HIGH);
    isPumpRunnnig = true;
    warmFloor.isPump = true;
    timeLeftDisplay();
    warmFloor.showCurrentTime = false;
    if (sendScreen) {
      wifiClient.println(constructorCommand("pumpOn"));
      warmFloor.readScreen(wifiClient, true);
    }

}
int timerHowLongBoilerRun;
void WarmFloor::startBoiler(bool sendScreen, bool updateScreen) {
    timerHowLongBoilerRun = millis();
    digitalWrite(G_HEATING, HIGH);
    isBoilerHeating = true;
    warmFloor.isHeating = true;
    timeLeftDisplay();
    warmFloor.showCurrentTime = false;
    if (sendScreen) {
      wifiClient.println(constructorCommand("heatingOn"));
      warmFloor.readScreen(wifiClient, true);
    }

}
void WarmFloor::shutdownPump(bool sendScreen, bool updateScreen) {
  digitalWrite(G_PUMP, LOW);
  isPumpRunnnig = false;
  if (updateScreen) {
    warmFloor.pumpOff();
    }
  if (isPumpRunnnig == false && isBoilerHeating == false) {
    warmFloor.showCurrentTime = true;
    //warmFloor.isPump = false;
    repairScreen();
  }
}
void WarmFloor::shutdownBoiler(bool sendScreen, bool updateScreen) {
  digitalWrite(G_HEATING, LOW);
  isBoilerHeating = false;
  if (isPumpRunnnig == false && isBoilerHeating == false) {
    warmFloor.showCurrentTime = true;
    //warmFloor.isHeating = false;
    repairScreen();
  }
}
volatile int timerTimeLeftPump;
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
    readScreen(wifiClient, false); // false = read and send with timer

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
    readScreen(wifiClient, false); // false = read and send with timer

  }
}
void WarmFloor::timeLeftDisplay() { // for first run mode
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
  // Draw bitmap on the screen
  g_display.setBitmapMode(1);
  uint8_t widthHeight = 24;
  if (warmFloor.isHeating) {
    WHITE
    g_display.drawXBM(0, 36, widthHeight, widthHeight, heating_map);
  } else {
    BLACK
    g_display.drawBox(0, 36, widthHeight, widthHeight);
  }
  }
  // Draw line
  g_display.drawBox(0,32,SCREEN_WIDTH, 2);
  g_display.sendBuffer();
  // wifi / connected
  warmFloor.wifiOn();
  warmFloor.isConnected(true);

}
void WarmFloor::timeLeftPumpDisplay(const char * timeLeft) {
    //xSemaphoreTake(xMutex, portMAX_DELAY);
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn); //u8g_font_unifont = 10px
    uint8_t clearLength = 102; //g_display.getStrWidth("00:00:00");
    uint8_t legntghTime = g_display.getStrWidth(timeLeft);
    uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
    uint8_t centerY = (SCREEN_HEIGHT / 2 - 12) / 2; // (128-24-60) / 2 + 24
    uint8_t centerYCursor = (SCREEN_HEIGHT / 2 + 12) / 2; // (128-24-60) / 2 + 24
    g_display.drawBox(25, centerY, clearLength, 12);
    WHITE
    g_display.setCursor(centerX, centerYCursor);  // Start at top-left corner
    g_display.print(F(timeLeft));
    g_display.sendBuffer();
   // xSemaphoreGive(xMutex);
}
void WarmFloor::timeLeftBoilerDisplay(const char * timeLeft) {
    //xSemaphoreTake(xMutex, portMAX_DELAY);
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn);
    uint8_t clearLength = 102; //g_display.getStrWidth("00:00:00");
    uint8_t legntghTime = g_display.getStrWidth(timeLeft);
    uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
    uint8_t centerY = (SCREEN_HEIGHT / 2 - 12) / 2 + 32; // (128-24-60) / 2 + 24
    uint8_t centerYCursor = (SCREEN_HEIGHT / 2 + 12) / 2 + 32; // (128-24-60) / 2 + 24
    g_display.drawBox(25, centerY, clearLength, 12); // 12 = height font
    WHITE
    g_display.setCursor(centerX, centerYCursor);  // Start at top-left corner
    g_display.print(F(timeLeft));
    g_display.sendBuffer();
   // xSemaphoreGive(xMutex);

}
const char *WarmFloor::twoDigitsTimeLeft(int timeLeft) {
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
    textHours = textHours + ":";
  }
  if (minutes < 10) {
    textMinutes = "0" + textMinutes + ":";
  } else {
    textMinutes = textMinutes + ":";
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
  isClient = isConnected;
}

void clearDisplay(){
  g_display.clearDisplay();
 // g_display.sendBuffer();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    //WiFi.begin(ssid, password);
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
  // if (millis() - timerCheckWifiOn >= updateInterval) {
    timerCheckWifiOn = millis();
    if (WiFi.status() != WL_CONNECTED) {
      // WiFi.disconnect(true);
      // WiFi.mode(WIFI_OFF);
      warmFloor.wifiOf();
    }
  // }
}

int ts = 0;
uint32_t timerDisplayTime;
void WarmFloor::setDispalyTime(int updateInterval) {
  if (showCurrentTime == true) {
  if (millis() - timerDisplayTime >= updateInterval) {
    timerDisplayTime = millis();  // сброс таймера
    //xSemaphoreTake(xMutex, portMAX_DELAY);
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn);
    uint8_t legnthTime = g_display.getStrWidth("00:00");
    uint8_t center = SCREEN_WIDTH / 2 - legnthTime / 2;
    g_display.drawBox(center, 15, legnthTime, 18);
    g_display.setCursor(center, 20);  // Start at top-left corner
    WHITE
    g_display.print(F(twoDigits()));
    g_display.sendBuffer();
    //xSemaphoreGive(xMutex);
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

uint32_t timerTickTime;
void WarmFloor::tickTime() {   // interval 1000
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
void WarmFloor::commands(WiFiClient client, const char commands[50]) {
  Serial.print("Commands On Core ");
  Serial.println(xPortGetCoreID());
  // commands = "pumpOff,765";  
  char *comma = strchr(commands, ',');
  char command[50] = "";
  long int value = 0; // how long work
  if(comma != NULL) {
  strncpy(command, commands, comma - commands);
   value = atol(++comma);
  } else {
      strcpy(command, commands);
  }
  Serial.print("command = ");
  Serial.println(command);

  if (strcmp(command, "pumpOn") == 0) {
    if (value > 0) {
      howLongPump = value;
      preferences.begin("timeSettings", false);
      preferences.putInt("howLongPump", value);
      Serial.println("Saved using Preferences");
      preferences.end();
    }
    warmFloor.startPump(true);    
  }
  if (strcmp(command, "pumpOff") == 0) {
    if (isBoilerHeating == false) {
      warmFloor.pumpOff();
    } else {
      // pumpOf icon
      g_display.setFont(u8g_font_unifont);
      WHITE
      g_display.drawRBox(0, 3, 24, 24, 5);
      BLACK
      g_display.drawRBox(3, 6, 18, 18, 5);
      g_display.setCursor(4, 20);
      WHITE
      g_display.print(F("Of"));
      // write text
      BLACK
      g_display.drawBox(24, 10, 104, 12);
      WHITE
      uint8_t legntghTime = g_display.getStrWidth("Pump off");
      uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
      g_display.setCursor(centerX, 20);
      g_display.print(F("Pump off"));
      warmFloor.isPump = false;
    }  
    warmFloor.shutdownPump();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("pumpOff"));
    readScreen(client, true);
  }
  if (strcmp(command, "heatingOn") == 0) {
    if (value > 0) {
      howLongBoiler = value;
      preferences.begin("timeSettings", false);
      preferences.putInt("howLongBoiler", value);
      Serial.println("Saved using Preferences");
      preferences.end();
    }
    warmFloor.startBoiler(true);
  }
  if (strcmp(command, "heatingOff") == 0) {
    if (isPumpRunnnig == false) {
      warmFloor.heating(false);
    } else {
      BLACK
      g_display.drawBox(30, 36, 128, 24);
      WHITE
      g_display.setFont(u8g_font_unifont);
      uint8_t legntghTime = g_display.getStrWidth("Heating off");
      uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
      g_display.setCursor(centerX, 54);
      g_display.print(F("Heating off"));
      warmFloor.isHeating = false;

    }
    warmFloor.shutdownBoiler();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("heatingOff"));
    readScreen(client, true);
  }
  if (strcmp(command, "updateTime") == 0) {
    internetTime.setTime();
    warmFloor.setDispalyTime();
    client.println(constructorCommand("updateTime"));
    readScreen(client, true);
  }
  if (strcmp(command, "setCurrentTime") == 0) {
    if (value > 0) {
    internetTime.setWatchTime(value); // set unixtime
    warmFloor.setDispalyTime();
    client.println(constructorCommand("setCurrentTime"));
    readScreen(client, true);
    }
  }
  if (strcmp(command, "readScreen") == 0) {
    readScreen(client, true);
    client.println(constructorCommand("readScreen"));
  }
  if (strcmp(command, "clearDisplay") == 0) {
    clearDisplay();
    client.println(constructorCommand("clearDisplay"));
  }
    if (strcmp(command, "repairScreen") == 0) {
    client.println(constructorCommand("repairScreen"));
    warmFloor.repairScreen();
  }
  if (strcmp(command, "wifiOff") == 0) {
    client.println(constructorCommand("wifiOff"));
    wifiServer.stop();
    WiFi.disconnect(true);
    WHITE
    g_display.setFont(u8g_font_unifont);
    uint8_t legntghTime = g_display.getStrWidth("Wif off");
    uint8_t centerX =
        (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24;  // (128-24-60) / 2 + 24
    g_display.setCursor(centerX, 32);
    g_display.print(F("Wif off"));
  }
  if (strcmp(command, "reboot") == 0) {
    client.println(constructorCommand("reboot"));
    ESP.restart();
  }
  if (strcmp(command, "getSettings") == 0) {
    warmFloor.preferences.begin("timeSettings", true);
    warmFloor.howLongPump = warmFloor.preferences.getInt("howLongPump", 15000);
    warmFloor.howLongBoiler = warmFloor.preferences.getInt("howLongBoiler", 25000);
    string str = to_string(warmFloor.howLongPump) + " " + to_string(warmFloor.howLongBoiler);
    client.println(constructorCommand("timeSettings",str));
    Serial.print("howLongPump ");
    Serial.println(howLongPump);
    Serial.print("howLongBoiler ");
    Serial.println(howLongBoiler);


  }
}

void WarmFloor::toCenterDisplay(const char * timeLeft) {
    //xSemaphoreTake(xMutex, portMAX_DELAY);
    BLACK
    g_display.setFont(u8g2_font_crox3cb_tn); //u8g_font_unifont = 10px
    uint8_t clearLength = 102; //g_display.getStrWidth("00:00:00");
    uint8_t legntghTime = g_display.getStrWidth(timeLeft);
    uint8_t centerX = (SCREEN_WIDTH - 24 - legntghTime) / 2 + 24; // (128-24-60) / 2 + 24
    uint8_t centerY = (SCREEN_HEIGHT / 2 - 12) / 2; // (128-24-60) / 2 + 24
    uint8_t centerYCursor = (SCREEN_HEIGHT / 2 + 12) / 2; // (128-24-60) / 2 + 24
    g_display.drawBox(25, centerY, clearLength, 12);
    WHITE
    g_display.setCursor(centerX, centerYCursor);  // Start at top-left corner
    g_display.print(F(timeLeft));
    g_display.sendBuffer();
   // xSemaphoreGive(xMutex);
}
int  timerOftenSend;
void WarmFloor::readScreen(WiFiClient client, bool sendNow) {
  if (sendNow) { timerOftenSend = 0; }
  if (millis() - timerOftenSend >= 1000) {
    timerOftenSend = millis();
    if (client.connected()) {  // maybe reset watchdog timer
      std::string pixels;
      Serial.println("start read");
      screen.getPixels(pixels);
      Serial.println("end read");
      client.println(constructorCommand("screen", pixels));

      // if (sendNow) {
      //   // Serial.println("send size = " + pixels.size());  // for some reason
      //   // it loops wdt
      // } else {
      //   Serial.println("Not send just read");
      // }
    } else {
      Serial.println("No client");
    }
  }
}
void WarmFloor::justConnected(WiFiClient client) {
  if (warmFloor.isPump) {
    client.println(constructorCommand("pumpOn"));
  } else {
    client.println(constructorCommand("pumpOff"));
  }
    if (warmFloor.isHeating) {
    client.println(constructorCommand("heatingOn"));
  } else {
    client.println(constructorCommand("heatingOff"));
  }
  client.println(constructorCommand("howLongPump", to_string(howLongPump)));
  client.println(constructorCommand("howLongBoiler", to_string(howLongBoiler)));

  warmFloor.readScreen(client, true);
}
void webServerBegin(void *pvParameters) {
  wifiServer.begin();
  Serial.print("Task On Core ");
  Serial.println(xPortGetCoreID());
  vTaskDelete(TaskWebserverBegin);
}
void webServer(void *pvParameters) {
  for (;;) {
    //xSemaphoreTake(xMutex, portMAX_DELAY);
    WiFiClient client = wifiServer.available();
    if (client) {
      wifiClient = client;
      createJustConnectedCore1();
    //  xSemaphoreTake(xMutex, portMAX_DELAY);
    //  xSemaphoreGive(xMutex);
      while (client.connected()) {
        while (client.available() > 0) {
          //xSemaphoreTake(xMutex, portMAX_DELAY);
          char symbol[2] = {0};
          symbol[0] = client.read(); // [2] for writing a terminating nul past the end of the destination
          strcat(commandWebServer, &symbol[0]);   // append and assignment
          if (symbol[0] == '\n') {  // если байт является переводом строки
            Serial.println("yes last contain");
            memset(wifiCommands, 0, sizeof(wifiCommands));  // clear
            char temp[20] = "";
            strncpy (temp, commandWebServer, strlen (commandWebServer) - 2); // for correct compare witout '\n
            strcpy(wifiCommands - strlen(wifiCommands), temp); // assigment to external commands
            //strcpy(wifiCommands - strlen(wifiCommands), commandWebServer); // assigment to external commands
            Serial.println(wifiCommands);
            createCommandsCore1(); // run command after terminating null
            Serial.println("Web server on core ");
            Serial.println(xPortGetCoreID());
            memset(commandWebServer, 0, sizeof(commandWebServer));  // clear
           // xSemaphoreGive(xMutex);

          }
        }
        delay(10);
      }

      client.stop();
      Serial.println("Client disconnected");
      createClientDisconnectedCore1();
    }
   // Serial.println("wifi check");
    //xSemaphoreGive(xMutex);
    //delay(10);

  }
}
void WarmFloor::taskToCore(){
    // create a task that will be executed in the Task1code() function, with
  // priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
      webServerBegin,       /* Task function. */
      "TaskWebserverBegin", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      4,               /* priority of the task */
      &TaskWebserverBegin,  /* Task handle to keep track of created task */
      0);  
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
  //delay(50);
  Serial.println("CommandsStart delete");

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