
#include "settings.h"
#include "defaults.h"

#include "EEPROM.h"

SettingsData settings;

/* Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the chromiumos LICENSE file.
 * Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A
 * table-based algorithm would be faster, but for only a few bytes it isn't
 * worth the code size. */
uint8_t crc8(const void* vptr, int len) {
  const uint8_t* data = (const uint8_t*)vptr;
  unsigned crc = 0;
  int i, j;
  for (j = len; j; j--, data++) {
    crc ^= (*data << 8);
    for (i = 8; i; i--) {
      if (crc & 0x8000)
        crc ^= (0x1070 << 3);
      crc <<= 1;
    }
  }
  return (uint8_t)(crc >> 8);
}
/* end copyright section */

const SettingsData defaults = {
  signature : SETTINGS_SIGNATURE,
  flags : (1 << 3) | (1 << 4),
  mode : 0,
  countsPerMm : DEFAULTS_COUNTS_PER_MM,
  countsPerDeg : DEFAULTS_COUNTS_PER_DEG,
  fwdKP : DEFAULTS_FWD_KP,
  fwdKD : DEFAULTS_FWD_KD,
  rotKP : DEFAULTS_ROT_KP,
  rotKD : DEFAULTS_ROT_KD,
  lineKP : DEFAULTS_LINE_KP,
  lineKD : DEFAULTS_LINE_KD,
  leftFFSpeedFwd : DEFAULTS_LEFT_FF_SPEED_FWD,
  leftFFSpeedRev : DEFAULTS_LEFT_FF_SPEED_REV,
  leftFFAccFwd : DEFAULTS_LEFT_FF_ACC_FWD,
  leftFFAccRev : DEFAULTS_LEFT_FF_ACC_REV,
  leftFFStaticFwd : DEFAULTS_LEFT_FF_STATIC_FWD,
  leftFFStaticRev : DEFAULTS_LEFT_FF_STATIC_REV,
  rightFFSpeedFwd : DEFAULTS_RIGHT_FF_SPEED_REV,
  rightFFSpeedRev : DEFAULTS_RIGHT_FF_SPEED_FWD,
  rightFFAccFwd : DEFAULTS_RIGHT_FF_ACC_FWD,
  rightFFAccRev : DEFAULTS_RIGHT_FF_ACC_REV,
  rightFFStaticFwd : DEFAULTS_RIGHT_FF_STATIC_FWD,
  rightFFStaticRev : DEFAULTS_RIGHT_FF_STATIC_REV,
  fudge : 999,
};

/****************************************************************************/
// REMEMBER to change the signature if the settings structure changes or badness will follow

uint8_t settingsChecksum(SettingsData& s) {
  return crc8(&s, sizeof(SettingsData));
}

void settingsRead() {
  const int ERR_CHECKSUM = 1;
  const int ERR_SIGNATURE = 2;
  uint8_t eepromChecksum;
  SettingsData s;
  EEPROM.get(EEPROM_ADDR_SETTINGS, s);
  EEPROM.get(EEPROM_ADDR_SETTINGS + sizeof(SettingsData), eepromChecksum);
  int error = 0;
  if (eepromChecksum != settingsChecksum(s)) {
    error += ERR_CHECKSUM;
  }
  if (s.signature != SETTINGS_SIGNATURE) {
    error += ERR_SIGNATURE;
  }
  if (error == 0) {
    settings = s;
    return;
  }
  if (error & ERR_CHECKSUM) {
    Serial.println(F(" - checksum error"));
  }
  if (error & ERR_SIGNATURE) {
    Serial.println(F(" - signature error"));
  }
  Serial.println(F("Using defaults"));
  settingsReset();
}

void settingsWrite() {
  EEPROM.put(EEPROM_ADDR_SETTINGS, settings);
  EEPROM.put(EEPROM_ADDR_SETTINGS + sizeof(SettingsData), settingsChecksum(settings));
}

void settingsReset() {
  settings = defaults;
  settingsWrite();
}

void settingsPrint() {
  Serial.println(F("Not yet implemented"));
}