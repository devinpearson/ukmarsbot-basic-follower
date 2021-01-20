#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "board.h"
#include "defaults.h"

struct SettingsData {
  uint8_t version;
  uint8_t flags;
  uint8_t mode;
  float mmPerCount;
  float degPerCount;
  float fwdKP;
  float fwdKD;
  float rotKP;
  float rotKD;
  float lineKP;
  float lineKD;
  float leftFFSpeedFwd;
  float leftFFSpeedRev;
  float leftFFAccFwd;
  float leftFFAccRev;
  float leftFFStaticFwd;
  float leftFFStaticRev;
  float rightFFSpeedFwd;
  float rightFFSpeedRev;
  float rightFFAccFwd;
  float rightFFAccRev;
  float rightFFStaticFwd;
  float rightFFStaticRev;
  float fudge;
};

extern SettingsData settings;
extern const SettingsData defaults;

/***
 * Settings are stored in the on-board EEPROM of the processor and retrieved into a working copy
 * held in RAM while the program is running.
 *
 * There is also a set of read-only default values compiled into the ROM image. The default values
 * used can all be found in the "defaults.h" file.
 *
 * When the software boots up, the EEPROM settings are read and examined. The EEPROM copy
 * has a checksum byte also stored in EEPROM. After the data has been read from EEPROM,
 * the read data has its checksum calculated.  If the calculated checksum matches the
 * stored checksum then the settings read from the EEPROM are copied into the working set
 * and program executions continues.
 *
 * If the signature in the EEPROM data does not match the signature of the defaults, then
 * it is assumed that the settings structure has changed. This causes the EEPROM data and the
 * working copy to be overwritten with the default values.
 *
 * This is all a bit involved but it means that the EEPROM settings are protected as long as
 * the signature does not change. Note that changing the Settings structure without also changing
 * the signature will cause unpredictable errors and undefined behaviour.
 *
 * Also, a change to the signature will force the settings stored in EEPROM to be overwritten
 * by the compiled defaults.
 *
 * Three support functions are provided:
 */

/***
 * Read the working settings from EEPROM.
 * Undoes changes
 * If the settings signature does not match defaults or checksums do not match
 * settings are reset to defaults and saved back to EEPROM
 */
void settingsRead();

/***
 * Store the current working settings to EEPROM
 */
void settingsWrite();

/***
 * Replace all settings with the compiled defaults and write them to EEPROM
 */
void settingsReset();

/***
 * Print the current working settings
 */
void settingsPrint();

uint8_t crc8(const void* data, int len);

#endif