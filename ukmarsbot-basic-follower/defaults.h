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

#ifdef UKMARSBOT_ARIADNE

const uint32_t BAUDRATE = 115200;
const float LOOP_INTERVAL = (1.0 / 500.0);

const int SENSOR_COUNT = 6;
const float MAX_MOTOR_VOLTS = 6.0;

const int EEPROM_ADDR_SETTINGS = 0x0000;
#define SETTINGS_SIGNATURE 0xF1F0C003
#define ENCODER_COUNTS_PER_ROTATION 36
#define GEAR_RATIO 20
#define WHEEL_DIAMETER 32.0  // mm
#define MOUSE_DIAMETER 76.0
#define COUNTS_PER_WHEEL_ROTATION (GEAR_RATIO * ENCODER_COUNTS_PER_ROTATION)
#define COUNTS_PER_WHEEL_MM (COUNTS_PER_WHEEL_ROTATION / (PI * WHEEL_DIAMETER))
#define DEFAULTS_COUNTS_PER_MM (2 * COUNTS_PER_WHEEL_MM)  // sum of left and right encoders for 1mm travel
#define DEFAULTS_COUNTS_PER_DEG (DEFAULTS_COUNTS_PER_MM*PI * MOUSE_DIAMETER / 360.0)
// #define DEFAULTS_COUNTS_PER_DEG 3.0799  // sum of left and right encoders for 360 degree turn
// #define DEFAULTS_COUNTS_PER_MM 4.6096                     // sum of left and right encoders for 1mm travel


#define DEFAULTS_FWD_KP 1.0
#define DEFAULTS_FWD_KD 5.0

#define DEFAULTS_ROT_KP 0.35
#define DEFAULTS_ROT_KD 4.0

#define DEFAULTS_MOUSE_RADIUS (57.29583 * DEFAULTS_COUNTS_PER_DEG / DEFAULTS_COUNTS_PER_MM)
// controller constants for the line follower configuration
#define DEFAULTS_LINE_KP 1.5
#define DEFAULTS_LINE_KD 6.0
#define DEFAULTS_LINE_ALPHA 0.1  // sensor error filter constant

// controller constants for the line follower configuration
#define DEFAULTS_WALL_KP 1.5
#define DEFAULTS_WALL_KD 6.0

// time delay for sensors to respond to emitters
#define DEFAULTS_EMITTER_ON_TIME 50

// Motor Feedforward
#define DEFAULTS_LEFT_FF_SPEED_FWD 0.00347
#define DEFAULTS_LEFT_FF_SPEED_REV 0.00347
#define DEFAULTS_LEFT_FF_ACC_FWD 0.000362
#define DEFAULTS_LEFT_FF_ACC_REV 0.000362
#define DEFAULTS_LEFT_FF_STATIC_FWD 0.317
#define DEFAULTS_LEFT_FF_STATIC_REV 0.317

#define DEFAULTS_RIGHT_FF_SPEED_FWD 0.00347
#define DEFAULTS_RIGHT_FF_SPEED_REV 0.00347
#define DEFAULTS_RIGHT_FF_ACC_FWD 0.000362
#define DEFAULTS_RIGHT_FF_ACC_REV 0.000362
#define DEFAULTS_RIGHT_FF_STATIC_FWD 0.317
#define DEFAULTS_RIGHT_FF_STATIC_REV 0.317

#else
#error "there must be a valid set of defaults"
#endif

#endif