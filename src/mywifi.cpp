#include <WiFi.h>
#include "mywifi.h"
#include <ESPmDNS.h>
#include "led.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

unsigned long lastReconnectAttempt = 0;
AsyncWebServer server(80);

bool isWiFiSet() {
  return LittleFS.exists("/wifi.json");
}

void WIFiConfig::startWiFiOrAP() {
  settings = loadWiFiSettings();
  if (!connectToWiFi() && !settings.connectedOnce) {
    Serial.println("❌ Nem sikerült csatlakozni a Wi-Fi-hez, indítás AP módban...");
    if (!settings.connectedOnce) Serial.println("❌... mert még eddig nem csatlakozott arra a hálózatra.");
    startAccessPoint();
  } 
  startMDNS();
}

void WIFiConfig::resetButtonHandle(int pressDuration) {
  if (pressDuration > 3000 && !apMode) { // 3 másodperc
      Serial.println("🆘 Gomb hosszú lenyomás – AP mód aktiválása!");
      
      WiFi.disconnect(true);
      delay(500);
      apMode = true;
      resetwifi = true;
      startAccessPoint();
    }
}

WifiSettings WIFiConfig::loadWiFiSettings() {
  WifiSettings settings = {"", "", false};

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
void WIFiConfig::saveWiFiSettings(bool connectedOnce) {
  JsonDocument doc;
  doc["ssid"] = settings.ssid;
  doc["password"] = settings.password;
  doc["connectedOnce"] = connectedOnce;

  File file = LittleFS.open("/wifi.json", "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

bool WIFiConfig::connectToWiFi() {
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
        settings.connectedOnce = true;
        saveWiFiSettings(true);
      }
      apMode = false;
      return true;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n❌ Sikertelen kapcsolódás.");
  ledSetColor(true, false, false);
  return false;
}

void WIFiConfig::reconnectIfNeeded() {
  if (!resetwifi && settings.connectedOnce) {
    if (WiFi.status() != WL_CONNECTED && millis() - lastReconnectAttempt > reconnectInterval) {
      Serial.println("🔁 Újrapróbálkozás Wi-Fi csatlakozásra...");
      lost_connection = true;
      WiFi.begin(settings.ssid.c_str(), settings.password.c_str());
      lastReconnectAttempt = millis();
    }
    if (lost_connection && WiFi.status() == WL_CONNECTED) {
      Serial.println("✅ Csatlakozva: " + WiFi.localIP().toString());
      ledSetColor(false, true, false);
      if (!settings.connectedOnce) {
        saveWiFiSettings(true);
      }
      lost_connection = false;
      apMode = false;
    }
  }
}

void WIFiConfig::startAccessPoint() {
  Serial.println("AP mód aktiválása: vilagitas-setup");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(configSsid);
  Serial.println("🌐 AP mód: http://" + WiFi.softAPIP().toString());
  apMode = true;
  ledSetColor(false, false, true); // Kék = AP mód
}

void WIFiConfig::startMDNS() {
  if (MDNS.begin(dnsName)) Serial.println("🌍 mDNS: http://"+String(dnsName)+".local");
  else Serial.println("❌ mDNS hiba");
}

void WIFiConfig::setupWebServer() {
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!this->apMode && this->settings.connectedOnce && !this->resetwifi) {
      Serial.println("🌐 Wi-Fi beállítva, kiszolgáló indítása...");
      request->send(LittleFS, "/index.html", "text/html");
    } else {
      request->send(LittleFS, "/wifi.html", "text/html");
    }
  });

  server.on("/savewifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (this->apMode && (!this->settings.connectedOnce || this->resetwifi)) {
      if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();
        
        this->resetwifi = false;
        settings = {ssid, password, false}; // Reset the connectedOnce flag
        saveWiFiSettings(false); // Save the new settings without marking as connectedOnce
        request->send(200, "text/plain; charset=utf-8", "✅ Mentve, próbál csatlakozni...");

        ledSetColor(false, false, true);
        delay(500);
        ESP.restart();
      } else {
        request->send(400, "text/plain; charset=utf-8", "Hiányzó mező.");
      }
    } else {
      request->send(500, "text/plain; charset=utf-8", "Az oldal nem található (legalább is ha már be van állítva a WiFi).");
    }
  });

  server.begin();
}
