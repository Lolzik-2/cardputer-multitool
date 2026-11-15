// wifi/wifi_atks.h
#pragma once
#include <Arduino.h>

struct WifiTarget {
  String ssid;
  uint8_t bssid[6];
  int channel;
};

void wifiAtksSetup();
int scanWiFiTargets(WifiTarget* out, int maxCount);
void wifiStartTargetDeauth(int index);
void wifiStartDeauthFlood();
void wifiStopAttack();
void wifiAttackLoop();
