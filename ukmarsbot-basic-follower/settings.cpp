
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
  fwdKI : DEFAULTS_FWD_KI,
  fwdKD : DEFAULTS_FWD_KD,
  rotKP : DEFAULTS_ROT_KP,
  rotKI : DEFAULTS_ROT_KI,
  rotKD : DEFAULTS_ROT_KD,
  lineKP : DEFAULTS_LINE_KP,
  lineKD : DEFAULTS_LINE_KD,
  wallKP : DEFAULTS_WALL_KP,
  wallKD : DEFAULTS_WALL_KD,
  forward_ff : DEFAULTS_SPEED_FF,
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