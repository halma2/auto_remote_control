#include <Arduino.h>

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

struct WifiSettings {
  String ssid;
  String password;
  bool connectedOnce;
};

class WIFiConfig {
  WifiSettings settings;
  WifiSettings loadWiFiSettings();

  bool apMode = false;
  bool resetwifi = false;
  const String configSsid;
  const String dnsName;
  const unsigned long reconnectInterval;

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
