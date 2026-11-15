#include <M5Cardputer.h>
#include "settings.h"
#include "ir/ir_jammer.h"
#include "ir/tv_b_gone.h"
#include "wifi/wifi_atks.h"
#include "ble/ble_spam.h"

enum Screen {
  MAIN_MENU,
  IR_MENU,
  WIFI_MENU,
  BLE_MENU,
  SETTINGS_MENU,
  WIFI_TARGET_LIST,
  IR_JAMMER_RUNNING,
  TV_B_GONE_RUNNING,
  WIFI_ATTACK_RUNNING,
  BLE_SPAM_RUNNING
};

Screen currentScreen = MAIN_MENU;
int menuIndex = 0;
int wifiTargetIndex = 0;
int wifiTargetCount = 0;
WifiTarget wifiTargets[20];

const char* mainMenuItems[] = {"BLE", "IR", "WiFi", "Settings"};
const int MAIN_MENU_COUNT = 4;
const char* irMenuItems[] = {"IR Jammer", "TV-B-Gone", "Back"};
const int IR_MENU_COUNT = 3;
const char* wifiMenuItems[] = {"Target Atks", "Deauth Flood", "Back"};
const int WIFI_MENU_COUNT = 3;
const char* bleMenuItems[] = {"Samsung Spam", "Pixel Spam", "Back"};
const int BLE_MENU_COUNT = 3;
const char* settingsMenuItems[] = {"Brightness", "Battery", "Back"};
const int SETTINGS_MENU_COUNT = 3;

void drawMenu(const char* title, const char* items[], int count, int selected) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(ORANGE);
  M5Cardputer.Display.println(title);
  M5Cardputer.Display.drawLine(0, 28, 240, 28, ORANGE);
  M5Cardputer.Display.setTextSize(1);
  for (int i = 0; i < count; i++) {
    int y = 40 + i * 16;
    if (i == selected) {
      M5Cardputer.Display.fillRect(0, y - 2, 240, 14, ORANGE);
      M5Cardputer.Display.setTextColor(BLACK);
    } else {
      M5Cardputer.Display.setTextColor(WHITE);
    }
    M5Cardputer.Display.setCursor(4, y);
    M5Cardputer.Display.print(items[i]);
  }
}

void drawWiFiTargetList() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(ORANGE);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Targets");
  M5Cardputer.Display.drawLine(0, 28, 240, 28, ORANGE);
  M5Cardputer.Display.setTextSize(1);
  for (int i = 0; i < wifiTargetCount && i < 20; i++) {
    int y = 40 + i * 12;
    if (i == wifiTargetIndex) {
      M5Cardputer.Display.fillRect(0, y - 1, 240, 11, ORANGE);
      M5Cardputer.Display.setTextColor(BLACK);
    } else {
      M5Cardputer.Display.setTextColor(WHITE);
    }
    M5Cardputer.Display.setCursor(4, y);
    String name = wifiTargets[i].ssid;
    if (name.length() > 28) name = name.substring(0, 28);
    M5Cardputer.Display.print(name);
  }
}

void drawRunningScreen(const char* title, const char* status) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(ORANGE);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(title);
  M5Cardputer.Display.drawLine(0, 28, 240, 28, ORANGE);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.setCursor(4, 45);
  M5Cardputer.Display.println(status);
  M5Cardputer.Display.setCursor(4, 80);
  M5Cardputer.Display.println("Backspace = STOP");
}

// Навигация: используем ';' (вверх) и '.' (вниз), как на Cardputer
void handleMainMenu() {
  drawMenu("MAIN", mainMenuItems, MAIN_MENU_COUNT, menuIndex);
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { menuIndex = (menuIndex - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { menuIndex = (menuIndex + 1) % MAIN_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    switch (menuIndex) {
      case 0: currentScreen = BLE_MENU; menuIndex = 0; break;
      case 1: currentScreen = IR_MENU; menuIndex = 0; break;
      case 2: currentScreen = WIFI_MENU; menuIndex = 0; break;
      case 3: currentScreen = SETTINGS_MENU; menuIndex = 0; break;
    }
  }
}

void handleIRMenu() {
  drawMenu("IR", irMenuItems, IR_MENU_COUNT, menuIndex);
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { menuIndex = (menuIndex - 1 + IR_MENU_COUNT) % IR_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { menuIndex = (menuIndex + 1) % IR_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    if (menuIndex == 0) { irJammerStart(JAM_SWEEP); currentScreen = IR_JAMMER_RUNNING; }
    else if (menuIndex == 1) { tvBGoneStart(); currentScreen = TV_B_GONE_RUNNING; }
    else { currentScreen = MAIN_MENU; menuIndex = 0; }
  }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { currentScreen = MAIN_MENU; menuIndex = 0; }
}

void handleWiFiMenu() {
  drawMenu("WiFi", wifiMenuItems, WIFI_MENU_COUNT, menuIndex);
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { menuIndex = (menuIndex - 1 + WIFI_MENU_COUNT) % WIFI_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { menuIndex = (menuIndex + 1) % WIFI_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    if (menuIndex == 0) {
      wifiTargetCount = scanWiFiTargets(wifiTargets, 20);
      wifiTargetIndex = 0;
      currentScreen = WIFI_TARGET_LIST;
    } else if (menuIndex == 1) {
      wifiStartDeauthFlood();
      currentScreen = WIFI_ATTACK_RUNNING;
    } else {
      currentScreen = MAIN_MENU; menuIndex = 0;
    }
  }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { currentScreen = MAIN_MENU; menuIndex = 0; }
}

void handleBLESpamMenu() {
  drawMenu("BLE", bleMenuItems, BLE_MENU_COUNT, menuIndex);
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { menuIndex = (menuIndex - 1 + BLE_MENU_COUNT) % BLE_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { menuIndex = (menuIndex + 1) % BLE_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    if (menuIndex == 0) { bleSpamStart(BLE_SAMSUNG); currentScreen = BLE_SPAM_RUNNING; }
    else if (menuIndex == 1) { bleSpamStart(BLE_PIXEL); currentScreen = BLE_SPAM_RUNNING; }
    else { currentScreen = MAIN_MENU; menuIndex = 0; }
  }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { currentScreen = MAIN_MENU; menuIndex = 0; }
}

void handleSettingsMenu() {
  drawMenu("Settings", settingsMenuItems, SETTINGS_MENU_COUNT, menuIndex);
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { menuIndex = (menuIndex - 1 + SETTINGS_MENU_COUNT) % SETTINGS_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { menuIndex = (menuIndex + 1) % SETTINGS_MENU_COUNT; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    if (menuIndex == 0) {
      int b = getBrightness();
      b = (b >= 200) ? 50 : b + 50;
      setBrightness(b);
    } else if (menuIndex == 1) {
      // Обновление экрана для показа заряда
    } else {
      currentScreen = MAIN_MENU; menuIndex = 0;
    }
  }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { currentScreen = MAIN_MENU; menuIndex = 0; }
}

void handleWiFiTargetList() {
  drawWiFiTargetList();
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { wifiTargetIndex = (wifiTargetIndex - 1 + wifiTargetCount) % wifiTargetCount; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { wifiTargetIndex = (wifiTargetIndex + 1) % wifiTargetCount; delay(150); }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    wifiStartTargetDeauth(wifiTargetIndex);
    currentScreen = WIFI_ATTACK_RUNNING;
  }
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { currentScreen = WIFI_MENU; menuIndex = 0; }
}

void handleRunningScreen() {
  if (currentScreen == IR_JAMMER_RUNNING) {
    drawRunningScreen("IR Jammer", "Jamming...");
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { irJammerStop(); currentScreen = IR_MENU; menuIndex = 0; }
  } else if (currentScreen == TV_B_GONE_RUNNING) {
    drawRunningScreen("TV-B-Gone", "Sending codes...");
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { tvBGoneStop(); currentScreen = IR_MENU; menuIndex = 0; }
  } else if (currentScreen == WIFI_ATTACK_RUNNING) {
    drawRunningScreen("WiFi Attack", "Deauthing...");
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { wifiStopAttack(); currentScreen = WIFI_MENU; menuIndex = 0; }
  } else if (currentScreen == BLE_SPAM_RUNNING) {
    drawRunningScreen("BLE Spam", "Spamming...");
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) { bleSpamStop(); currentScreen = BLE_MENU; menuIndex = 0; }
  }
}

void setup() {
  M5Cardputer.begin();
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextColor(WHITE);
  loadSettings();
  irJammerSetup();
  tvBGoneSetup();
  wifiAtksSetup();
  bleSpamSetup();
}

void loop() {
  M5Cardputer.update();
  switch (currentScreen) {
    case MAIN_MENU: handleMainMenu(); break;
    case IR_MENU: handleIRMenu(); break;
    case WIFI_MENU: handleWiFiMenu(); break;
    case BLE_MENU: handleBLESpamMenu(); break;
    case SETTINGS_MENU: handleSettingsMenu(); break;
    case WIFI_TARGET_LIST: handleWiFiTargetList(); break;
    default: handleRunningScreen(); break;
  }
  if (currentScreen == IR_JAMMER_RUNNING) irJammerLoop();
  if (currentScreen == TV_B_GONE_RUNNING) tvBGoneLoop();
  if (currentScreen == WIFI_ATTACK_RUNNING) wifiAttackLoop();
  if (currentScreen == BLE_SPAM_RUNNING) bleSpamLoop();
  delay(5);
}
