#include "mywifi.h"
#include <Arduino.h>
#include "led.h"
#include <LittleFS.h>
#include <Wifi.h>

/* 
RGB led működése:
Kék
 villog - AP módban van az eszköz
 folyamatos - Valaki rákapcsolódott
Zöld
 villog - WiFi csatlakozás sikertelen
 folyamatos - WiFi csatlakozás sikeres
 Piros, sárga, mit tudom én...
 egyéb hiba - még ki kell találni.
*/

// Konfigurációs változók
const String dnsName = "vilagitas";
const String configSsid = "vilagitas-setup";
const unsigned long reconnectInterval = 10000; // 10 másodperc újracsatlakozási kísérlet

WIFiConfig wifiConfig = WIFiConfig(configSsid, dnsName, reconnectInterval);
const int buttonPin = 34; // vagy más szabad GPIO

void setup() {
  Serial.begin(115200);
  ledInit();
  pinMode(buttonPin, INPUT);

  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS indítása sikertelen");
    return;
  }
  Serial.println("✅ LittleFS indítva");
  wifiConfig.startWiFiOrAP();
  wifiConfig.setupWebServer();
}

unsigned long buttonPressStart = 0;
bool buttonHeld = false;
void watchButtonPress() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && !buttonHeld) {
    buttonPressStart = millis();
    buttonHeld = true;
  }
  if (buttonState == HIGH && buttonHeld) {
    wifiConfig.resetButtonHandle(millis() - buttonPressStart);
  }
  if (buttonState == LOW && buttonHeld) buttonHeld = false;
}

void loop() {
  wifiConfig.reconnectIfNeeded();
  
  watchButtonPress();

  // Egyéb működés...
  delay(50); // kis késleltetés a loop végén
}
