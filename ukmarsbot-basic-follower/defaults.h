#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "config.h"

/***
 * The defaults.h file holds the default settings values for the robot.
 * Here you can have different settings for individual robot variants. For
 * example, a variants with different gear ratios or wheel sizes
 *
 * The defaults shown here are compiled into the constant default Settings structure.
 * Normally, at start up there will be a copy of the last used settings structure
 * stored in EEPROM which is copied into the settings structure in RAM. If no valid
 * settings structure is found in EEPROM, the flash settings data is copied into
 * EEPROM and then loaded into RAM.
 * *
 * This cumbersome process makes it possible to have settings changed while the robot
 * is running and then save those settings to EEPROM so that they survive a reset.
 *
 * Remember always that just connecting the USB to the computer will cause a reset so
 * take care to use the auxiiary serial port (and possibly bluetooth) to make these
 * kinds of modifications.
 *
 * see the "settings.h" file for more details.
 */

#if defined(UKMARSBOT_ARIADNE)
#include "defaults-ariadne.h"
#elif defined(UKMARSBOT_DOROTHY)
#include "defaults-dorothy.h"
#else
#error "there must be a valid set of defaults"
#endif

#endif