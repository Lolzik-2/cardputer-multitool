#include "ble_spam.h"
#include <NimBLEDevice.h>
#include <esp_mac.h>

static NimBLEAdvertising* pAdvertising = nullptr;
static bool spamRunning = false;
static BleSpamType spamType = BLE_SAMSUNG;
static unsigned long lastAdv = 0;
static uint8_t mac[6];

void bleSpamSetup() {
  NimBLEDevice::init("");
  pAdvertising = NimBLEDevice::getAdvertising();
}

void bleSpamStart(BleSpamType type) {
  spamType = type;
  spamRunning = true;
  lastAdv = 0;
}

void bleSpamStop() {
  spamRunning = false;
  if (pAdvertising) pAdvertising->stop();
}

bool isBleSpamRunning() {
  return spamRunning;
}

void bleSpamLoop() {
  if (!spamRunning) return;
  if (millis() - lastAdv < 100) return;
  lastAdv = millis();

  // Ротация MAC-адреса для обхода фильтрации
  for (int i = 0; i < 6; i++) mac[i] = random(256);
  mac[0] |= 0xC0; // locally administered address

  NimBLEAdvertisementData advData;
  if (spamType == BLE_SAMSUNG) {
    advData.setName("Galaxy Watch");
  } else {
    advData.setName("Pixel");
  }

  pAdvertising->stop();
  esp_base_mac_addr_set(mac); // замена NimBLEDevice::setOwnAddr
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->start();
}
