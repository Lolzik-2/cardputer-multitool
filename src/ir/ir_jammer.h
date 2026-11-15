// ir/ir_jammer.h
#pragma once
#include <Arduino.h>

enum JamMode {
  JAM_BASIC,
  JAM_ENHANCED_BASIC,
  JAM_SWEEP,
  JAM_RANDOM,
  JAM_EMPTY
};

void irJammerSetup();
void irJammerStart(JamMode mode);
void irJammerStop();
void irJammerLoop();
bool isIrJammerRunning();
