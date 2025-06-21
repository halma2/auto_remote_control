#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "led.h"
#include "webserver.h"
#include "mywifi.h"

//pio run --target uploadfs
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
WiFiSettings settings;
const int buttonPin = 34; // vagy más szabad GPIO

bool wifiSettingsExists = false; // jelzi, hogy van-e Wi-Fi beállítás
AsyncWebServer server(80);


void setup() {
  Serial.begin(115200);
  ledInit();
  pinMode(buttonPin, INPUT);  // belső felhúzó

  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS indítása sikertelen");
    return;
  }
  Serial.println("✅ LittleFS indítva");
  wifiSettingsExists = LittleFS.exists("/wifi.json");
  settings = loadWiFiSettings();

  if (!connectToWiFi(settings) && !settings.connectedOnce) {
    Serial.println("❌ Nem sikerült csatlakozni a Wi-Fi-hez, indítás AP módban...");
    startAccessPoint();
  } 
  else {
    startMDNS();
  }

  setupWebServer();  // lásd lentebb
}

void loop() {
  static unsigned long buttonPressStart = 0;
  static bool buttonHeld = false;

  if (wifiSettingsExists && settings.connectedOnce) {
    reconnectIfNeeded(settings); // háttér Wi-Fi újracsatlakozás, ha kell
  } /*else {
    dnsServer.processNextRequest();
  }*/

  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && !buttonHeld) {
    buttonPressStart = millis();
    buttonHeld = true;
  }

  if (buttonState == HIGH && buttonHeld) {
    if (millis() - buttonPressStart > 3000 && !apModeTriggered) { // 3 másodperc
      Serial.println("🆘 Gomb hosszú lenyomás – AP mód aktiválása!");
      
      WiFi.disconnect(true);
      delay(500);
      apModeTriggered = true;
      startAccessPoint();
    }
  }

  if (buttonState == LOW && buttonHeld) {
    buttonHeld = false;
  }

  // Egyéb működés...
  delay(50); // kis késleltetés a loop végén
}
