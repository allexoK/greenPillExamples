#include <Arduino.h>
#include "pinmap.h"

void setup() {
  pinMode(P0_30,OUTPUT);
}
void loop() {
  digitalWrite(P0_30,HIGH);
  delay(100);
  digitalWrite(P0_30,LOW);
  delay(100);
}