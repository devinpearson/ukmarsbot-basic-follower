
#include "settings.h"
#include "defaults.h"

#include "EEPROM.h"

SettingsData settings;

const SettingsData defaults = {
  version : SETTINGS_VERSION,
  flags : (1 << 3) | (1 << 4),
  mode : DEFAULTS_ROBOT_MODE,
  mmPerCount : DEFAULTS_MM_PER_COUNT,
  degPerCount : DEFAULTS_DEG_PER_COUNT,
  fwdKP : DEFAULTS_FWD_KP,
  fwdKD : DEFAULTS_FWD_KD,
  rotKP : DEFAULTS_ROT_KP,
  rotKD : DEFAULTS_ROT_KD,
  lineKP : DEFAULTS_LINE_KP,
  lineKD : DEFAULTS_LINE_KD,
  wallKP : DEFAULTS_WALL_KP,
  wallKD : DEFAULTS_WALL_KD,
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

void settingsRead() {
  SettingsData s;
  EEPROM.get(EEPROM_ADDR_SETTINGS, s);
  if (s.version != SETTINGS_VERSION) {
    Serial.println(F("version change - settings to defaults"));
    settingsReset();
  } else {
    settings = s;
  }
}

void settingsWrite() {
  EEPROM.put(EEPROM_ADDR_SETTINGS, settings);
}

void settingsReset() {
  settings = defaults;
  settingsWrite();
}

void settingsPrint() {
  Serial.println(F("Not yet implemented"));
}