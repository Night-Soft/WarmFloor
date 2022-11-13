// include "WarmFloor.h"
// WarmFloor warmFloor;

// esp 32 spi = mosi-23 miso-19 clk-18 cs-5
// #define mosi 23;
// #define miso 19;
// #define clk 18;
// #define cs 5;

#define G_PUMP_BTN 3 // read btn


#include "Ex.h"
TaskHandle_t TaskMain;
struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button pumpButton = {G_PUMP_BTN, 0, false};
int timerIsrStartPump = 0;
void IRAM_ATTR isrStartPump() {
  if (millis() - timerIsrStartPump >= 300) {
    timerIsrStartPump = millis();
    Serial.println("startPump ");
        warmFloor.startPump(true);    

  }
  Serial.println("Exit isr");
}
int timerIsrShutdownPump = 0;
void IRAM_ATTR isrShutdownPump() {
  if (millis() - timerIsrShutdownPump >= 300) {
    timerIsrShutdownPump = millis();
    Serial.println("shutdownPump");
  }
  Serial.println("Exit isr");
}

void mainLoop(void *pvParameters);
void mainTaskBegin() {
  xTaskCreatePinnedToCore(
      mainLoop,       /* Task function. */
      "MainTaskBegin", /* name of task. */
      20000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      0,               /* priority of the task */
      &TaskMain,  /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */
}

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  // pinMode(G_PUMP, OUTPUT);
  // pinMode(G_HEATING, OUTPUT);
  // digitalWrite(G_PUMP, LOW);
  // digitalWrite(G_HEATING, LOW);
  // pinMode(pumpButton.PIN, INPUT_PULLDOWN);
  // attachInterrupt(pumpButton.PIN, isr, CHANGE);

  // pinMode(1, INPUT_PULLDOWN);
  // attachInterrupt(1, isr, CHANGE);
  // pinMode(2, INPUT_PULLDOWN);
  //  attachInterrupt(2, isr, CHANGE);
  // // pinMode(15, INPUT_PULLDOWN);
  //  attachInterrupt(15, isr, CHANGE);

  warmFloor.begin();
  
  if (warmFloor.isPump) {
    warmFloor.pumpOn();
  } else {
    warmFloor.pumpOff();
  }
  // pinMode(4, INPUT_PULLDOWN);
  // attachInterrupt(digitalPinToInterrupt(4), isrStartPump, HIGH);
  //attachInterrupt(digitalPinToInterrupt(4), isrShutdownPump, LOW);
  //esp_intr_reserve(4,1);

  //mainTaskBegin();

}

void loop() {
    warmFloor.tickTime();  // interval 1000
    warmFloor.setDispalyTime();
    // Serial.print("Time has passed ");
    // Serial.println(millis());
    //  if (warmFloor.isNeedPumpStart) {
    //    warmFloor.startPump();
    //  }
    if (warmFloor.isPumpRunnnig) {
      warmFloor.timeLeftPump(warmFloor.howLongPump); 
    }
    if (warmFloor.isBoilerHeating) {
      warmFloor.timeLeftBoiler(warmFloor.howLongBoiler); 
    }
    // if (pumpButton.pressed) {
    //   Serial.printf("Button 1 has been pressed %u times\n",
    //                 pumpButton.numberKeyPresses);
    //   pumpButton.pressed = false;
    // }
     //delay(10);
}
