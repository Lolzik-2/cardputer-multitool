// ir/ir_jammer.cpp
#include "ir_jammer.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_TX_PIN 44

static IRsend irsend(IR_TX_PIN);
static JamMode currentMode = JAM_BASIC;
static bool running = false;
static uint32_t jamCount = 0;

// Частоты, как в Bruce: 30 кГц – 56 кГц
const uint16_t IR_FREQUENCIES[] = {30000, 33000, 36000, 38000, 40000, 56000};
const uint8_t FREQ_COUNT = sizeof(IR_FREQUENCIES) / sizeof(IR_FREQUENCIES[0]);

void irJammerSetup() {
  irsend.begin();
  running = false;
}

void irJammerStart(JamMode mode) {
  currentMode = mode;
  running = true;
  jamCount = 0;
}

void irJammerStop() {
  running = false;
}

bool isIrJammerRunning() {
  return running;
}

void irJammerLoop() {
  if (!running) return;

  uint16_t freq = IR_FREQUENCIES[jamCount % FREQ_COUNT];

  switch (currentMode) {
    case JAM_BASIC:
      irsend.sendRaw(nullptr, 0, freq);
      delay(5);
      break;

    case JAM_ENHANCED_BASIC:
      irsend.mark(500);
      irsend.space(500);
      break;

    case JAM_SWEEP: {
      uint32_t mark = 200 + (jamCount % 800);
      irsend.mark(mark);
      irsend.space(mark);
      break;
    }

    case JAM_RANDOM:
      irsend.mark(random(100, 1000));
      irsend.space(random(100, 1000));
      break;

    case JAM_EMPTY:
      irsend.sendRaw(nullptr, 0, freq);
      break;
  }

  jamCount++;
  delay(1);
}
