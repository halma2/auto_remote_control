#include <Arduino.h>

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WIFiConfig {
  struct WifiSettings {
    String ssid;
    String password;
    bool connectedOnce;
  } settings;
  
  bool apMode = false;
  bool resetwifi = false;
  bool lost_connection = false;
  const String configSsid;
  const String dnsName;
  const unsigned long reconnectInterval;

  void loadWiFiSettings();
  void saveWiFiSettings(bool connectedOnce);
  bool connectToWiFi();
  void startAccessPoint();
  void startMDNS();
  bool isWiFiSet();

public:
  WIFiConfig(const String &ssid, const String &dnsName, const unsigned long reconnectInterval): 
    configSsid(ssid), dnsName(dnsName), reconnectInterval(reconnectInterval) {}
  void startWiFiOrAP();
  void setupWebServer();
  void reconnectIfNeeded();
  void resetButtonHandle(int pressDuration);
};

#endif
