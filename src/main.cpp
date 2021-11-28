//#include "WarmFloor.h"
// WarmFloor warmFloor;
#include "Ex.h"

void setup() {
  Serial.begin(9600);
  Serial.println("work");
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
 // delay(50);
}