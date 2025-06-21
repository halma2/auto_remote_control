#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "led.h"
#include "mywifi.h"

extern AsyncWebServer server;

bool isAuthSet() {
  return LittleFS.exists("/auth.json");
}

bool isWiFiSet() {
  return LittleFS.exists("/wifi.json");
}

// Felhasználói adatok mentése auth.json-be
void saveAuth(String username, String password) {
  File f = LittleFS.open("/auth.json", "w");
  if (f) {
    f.print("{\"username\":\"" + username + "\", \"password\":\"" + password + "\"}");
    f.close();
  }
}

// Webszerver útvonalak beállítása
void setupWebServer() {
  // Kis statikus fájlok, CSS, JS ha később kell
  

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (isWiFiSet() && !apModeTriggered) {
      Serial.println("🌐 Wi-Fi beállítva, kiszolgáló indítása...");
      request->send(LittleFS, "/index.html", "text/html");
    } else {
      request->send(LittleFS, "/wifi.html", "text/html");
    }
  });

  // ---- 1. Ha nincs Wi-Fi beállítva → wifi.html ----
  if (!isWiFiSet() || apModeTriggered ) {
    // DNS szerver beállítása: minden domaint ide irányít
    //dnsServer.start(53, "*", WiFi.softAPIP());

    /*server.onNotFound([](AsyncWebServerRequest *request) {
      request->redirect("/");
    });
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *r){ r->redirect("/"); });
    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *r){ r->redirect("/"); });*/


    server.on("/savewifi", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();

        saveWiFiSettings({ssid, password, false});
        request->send(200, "text/plain; charset=utf-8", "✅ Mentve, próbál csatlakozni...");
        
        ledSetColor(false, false, true); // Kék = AP mód

        delay(500);
        ESP.restart();
      } 
      else {
        request->send(400, "text/plain; charset=utf-8", "Hiányzó mező.");
      }
    });

    server.begin();
    return;
  }

  // ---- 2. Ha nincs jelszó beállítva → login.html ----
  if (!isAuthSet()) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/login.html", "text/html");
    });

    server.on("/setauth", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (request->hasParam("username", true) && request->hasParam("password", true)) {
        String user = request->getParam("username", true)->value();
        String pass = request->getParam("password", true)->value();

        saveAuth(user, pass);

        request->send(200, "text/html; charset=utf-8", "Jelszó elmentve. Indítsd újra az eszközt!");
      } else {
        request->send(400, "text/plain; charset=utf-8", "Hiányzó mező.");
      }
    });

    server.begin();
    return;
  }

  // ---- 3. Ha minden megvan → index.html ----
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.begin();
}
