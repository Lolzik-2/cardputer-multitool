// settings.cpp
#include "settings.h"
#include <Preferences.h>

static Preferences prefs;

void loadSettings() {
  prefs.begin("settings", false);
  int bright = prefs.getInt("bright", 150);
  M5Cardputer.Display.setBrightness(bright);
  prefs.end();
}

void setBrightness(int level) {
  if (level < 10) level = 10;
  if (level > 255) level = 255;
  M5Cardputer.Display.setBrightness(level);
  prefs.begin("settings", false);
  prefs.putInt("bright", level);
  prefs.end();
}

int getBrightness() {
  prefs.begin("settings", true);
  int b = prefs.getInt("bright", 150);
  prefs.end();
  return b;
}

int getBatteryLevel() {
  return M5Cardputer.Power.getBatteryLevel();
}
