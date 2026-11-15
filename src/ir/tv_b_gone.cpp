// ir/tv_b_gone.cpp
#include "tv_b_gone.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <M5Cardputer.h>

#define IR_TX_PIN 44

static IRsend irsend(IR_TX_PIN);
static bool active = false;
static int codeIndex = 0;
static unsigned long lastSend = 0;

// Примерный набор популярных NEC-кодов питания (можно расширить из WORLD_IR_CODES.h Bruce)
const uint32_t POWER_CODES[] = {
  0x20DF10EF,  // LG
  0xE0E040BF,  // Samsung
  0x10EFD827,  // Sony
  0x61A0F00F,  // Panasonic
  0x2FD48B7,   // Philips
  0x1CE348BF,  // Toshiba
  0x20DF8877,  // LG (альтернативный)
  0xE0E0E01F   // Samsung (альтернативный)
};
const int CODE_COUNT = sizeof(POWER_CODES) / sizeof(POWER_CODES[0]);

void tvBGoneSetup() {
  irsend.begin();
  active = false;
}

void tvBGoneStart() {
  active = true;
  codeIndex = 0;
  lastSend = 0;
}

void tvBGoneStop() {
  active = false;
}

void tvBGoneLoop() {
  if (!active) return;
  if (millis() - lastSend < 200) return;
  lastSend = millis();

  irsend.sendNEC(POWER_CODES[codeIndex], 32);
  codeIndex = (codeIndex + 1) % CODE_COUNT;
}
