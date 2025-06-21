#include <WiFi.h>
#include "mywifi.h"
#include <ESPmDNS.h>
#include "led.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

unsigned long lastReconnectAttempt = 0;
const char* dnsName = "vilagitas";
const char* configSsid = "vilagitas-setup";
extern const unsigned long reconnectInterval = 3000; // 30 másodperc
bool apModeTriggered = false;
/*const byte DNS_PORT = 53;
const char* captivePortalHostname = "setup.local"; // bármi lehet
DNSServer dnsServer;*/

WiFiSettings loadWiFiSettings() {
  WiFiSettings settings;

  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS inicializálása sikertelen.");
    return settings;
  }

  File file = LittleFS.open("/wifi.json", "r");
  if (!file) {
    Serial.println("❌ Nem található a wifi.json fájl.");
    return settings;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("❌ Hiba a JSON beolvasása közben:");
    Serial.println(error.c_str());
    return settings;
  }

  settings.ssid = doc["ssid"].as<String>();
  settings.password = doc["password"].as<String>();
  settings.connectedOnce = doc["connectedOnce"];
  Serial.println("📥 Wi-Fi beállítások betöltve:");
  Serial.println("SSID: " + settings.ssid);
  // Ne írd ki a jelszót biztonsági okból
  file.close();
  return settings;
}

// Wi-Fi adatok mentése wifi.json-be
void saveWiFiSettings(const WiFiSettings& settings) {
  JsonDocument doc;
  doc["ssid"] = settings.ssid;
  doc["password"] = settings.password;
  doc["connectedOnce"] = settings.connectedOnce;

  File file = LittleFS.open("/wifi.json", "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

void markConnectedOnce(WiFiSettings& settings) {
  JsonDocument doc;
  doc["ssid"] = settings.ssid;
  doc["password"] = settings.password;
  doc["connectedOnce"] = true;

  File file = LittleFS.open("/wifi.json", "w");
  if (file) {
    serializeJsonPretty(doc, file);
    file.close();
    Serial.println("✅ Mentve: connectedOnce = true");
    Serial.println("✅ Wi-Fi kapcsolat mentve (connectedOnce = true)");
  }
}

bool connectToWiFi(WiFiSettings& settings) {
  if (settings.ssid == "" || settings.password == "") {
    Serial.println("❌ Hiányzó Wi-Fi adatok.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid.c_str(), settings.password.c_str());

  Serial.print("🔌 Csatlakozás a Wi-Fi-hez... ");

  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("✅ Csatlakozva: " + WiFi.localIP().toString());
      ledSetColor(false, true, false);
      if (!settings.connectedOnce) {
        markConnectedOnce(settings);
      }
      return true;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n❌ Sikertelen kapcsolódás.");
  ledSetColor(true, false, false);
  return false;
}

void reconnectIfNeeded(WiFiSettings& wifiSettings) {
  if (WiFi.status() != WL_CONNECTED && millis() - lastReconnectAttempt > reconnectInterval) {
    Serial.println("🔁 Újrapróbálkozás Wi-Fi csatlakozásra...");
    WiFi.begin(wifiSettings.ssid.c_str(), wifiSettings.password.c_str());
    lastReconnectAttempt = millis();
  }
}


void startAccessPoint() {
  Serial.println("AP mód aktiválása: vilagitas-setup");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(configSsid);
  Serial.println("🌐 AP mód: http://" + WiFi.softAPIP().toString());

  ledSetColor(false, false, true); // Kék = AP mód

  // DNS szerver: minden lekérdezést az AP IP-jére irányít
  //dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

void startMDNS() {
  if (MDNS.begin(dnsName)) Serial.println("🌍 mDNS: http://"+String(dnsName)+".local");
  else Serial.println("❌ mDNS hiba");
}