// ble/ble_spam.cpp
#include "ble_spam.h"
#include <NimBLEDevice.h>

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
}

void bleSpamStop() {
  spamRunning = false;
  if (pAdvertising) pAdvertising->stop();
}

void bleSpamLoop() {
  if (!spamRunning) return;
  if (millis() - lastAdv < 100) return;
  lastAdv = millis();

  // Ротация MAC для обхода фильтрации
  esp_random(); // инициализация
  for (int i = 0; i < 6; i++) mac[i] = random(256);
  mac[0] |= 0xC0; // locally administered

  NimBLEAdvertisementData advData;

  if (spamType == BLE_SAMSUNG) {
    // FastPair для Galaxy Watch / Buds
    advData.setName("Galaxy Watch");
    // В реальном Bruce используются конкретные manufacturer-specific data.
    // Здесь — базовый пакет для триггера уведомлений.
  } else {
    // Google FastPair для Pixel
    advData.setName("Pixel");
  }

  pAdvertising->stop();
  NimBLEDevice::setOwnAddr(mac);
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->start();
}
