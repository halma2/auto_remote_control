#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
//#include <DNSServer.h>

//Manuális Wi-Fi kezelő osztály
extern unsigned long lastReconnectAttempt;
extern const unsigned long reconnectInterval; // 30 másodperc
extern const char* dnsName; // mDNS név
extern const char* configSsid; // AP SSID
extern bool apModeTriggered;

/*extern DNSServer dnsServer;
extern const byte DNS_PORT;
extern const char* captivePortalHostname;*/
struct WiFiSettings {
  String ssid;       // Wi-Fi SSID
  String password;   // Wi-Fi jelszó
  bool connectedOnce; // Előzőleg sikeresen csatlakozott-e
};
WiFiSettings loadWiFiSettings(); // Betölti a Wi-Fi beállításokat
bool connectToWiFi(WiFiSettings& settings);   // Próbál csatlakozni a beállított Wi-Fi-re
void reconnectIfNeeded(WiFiSettings& wifiSettings); // Újrapróbálkozik a Wi-Fi kapcsolódással, ha szükséges
void saveWiFiSettings(const WiFiSettings& settings); // Mentés LittleFS-be
void startAccessPoint(); // Elindítja az AP módot
void startMDNS();        // Elindítja az mDNS szolgáltatást

#endif