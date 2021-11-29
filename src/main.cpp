//#include "WarmFloor.h"
// WarmFloor warmFloor;
#include "Ex.h"

void setup() {
  Serial.begin(9600);
  Serial.println("work");
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  warmFloor.begin();
  warmFloor.heating(true);
  warmFloor.taskToCore();
  if (warmFloor.isPump) {
    warmFloor.pumpOn();
  } else {
    warmFloor.pumpOf();
  }
}

void loop() {
   warmFloor.tickTime();  // interval 1000
  warmFloor.setDispalyTime();
   //Serial.print("Time has passed ");
   //Serial.println(millis());
  //  if (warmFloor.isNeedPumpStart) {
  //    warmFloor.startPump();
  //  }
  if (warmFloor.isPumpRunnnig) {
    warmFloor.timeLeftPump(15000);
  }
  if (warmFloor.isBoilerHeating) {
    warmFloor.timeLeftBoiler(25000);
  }
    // delay(3000);
  }