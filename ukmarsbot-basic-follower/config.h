#ifndef CONFIG_H
#define CONFIG_H

#include <arduino.h>

/***
 * The config.h file defines the actual robot variant which will be the target for the build.
 *
 * A 'proper' build setup would probably define this on the command line or in a make file.
 *
 * The other information in here holds system-wide configuration values that are not dependent
 * upon the target robot. That is things like the serial port baud rate and loop interval.
 *
 *
 */

// the robot board type - only define one
#define BOARD_UKMARSBOT_V1

// Robot variant so that the correct defaults can be selected - only define one
// #define UKMARSBOT_ARIADNE
#define UKMARSBOT_DOROTHY

const uint32_t BAUDRATE = 115200;

// sometimes the controller needs the interval, sometimes the frequency
// define one and pre-calculate the other. The compiler does the work and no flash or
// RAM storage is used. Constants are used for better type checking and traceability.
const float LOOP_FREQUENCY = 500.0;
const float LOOP_INTERVAL = (1.0 / LOOP_FREQUENCY);

#endif
