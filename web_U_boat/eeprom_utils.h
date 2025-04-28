#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <EEPROM.h>
#include "config.h"

void initEEPROM() {
  EEPROM.begin(512);
}

String readWiFiSSID() {
  String ssid;
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(i);
    if (c == 0) break;
    ssid += c;
  }
  return ssid;
}

String readWiFiPassword() {
  String password;
  for (int i = 32; i < 96; ++i) {
    char c = EEPROM.read(i);
    if (c == 0) break;
    password += c;
  }
  return password;
}

void writeWiFiCreds(String ssid, String password) {
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }
  EEPROM.write(ssid.length(), 0);
  
  for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(32 + i, password[i]);
  }
  EEPROM.write(32 + password.length(), 0);
  
  EEPROM.commit();
}

#endif