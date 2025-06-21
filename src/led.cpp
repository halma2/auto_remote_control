#include "led.h"
#include <Arduino.h>

void ledInit() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Kezdetben LED kikapcsolva
  ledSetColor(false, false, false);
}

void ledSetColor(bool red, bool green, bool blue) {
  // Fontos: ha katódközös LED-et használsz, akkor LOW = bekapcsolt
  digitalWrite(RED_PIN,   red   ? LOW : HIGH);
  digitalWrite(GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(BLUE_PIN,  blue  ? LOW : HIGH);
}