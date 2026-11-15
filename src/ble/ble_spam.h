// ble/ble_spam.h
#pragma once
#include <Arduino.h>

enum BleSpamType {
  BLE_SAMSUNG,
  BLE_PIXEL
};

void bleSpamSetup();
void bleSpamStart(BleSpamType type);
void bleSpamStop();
void bleSpamLoop();
