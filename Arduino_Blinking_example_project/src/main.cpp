#include <Arduino.h>
#include "pinmap.h"

void setup() {
  pinMode(P0_14,OUTPUT);
}
void loop() {
  digitalWrite(P0_14,HIGH);
  delay(100);
  digitalWrite(P0_14,LOW);
  delay(100);
}