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




// the robot board type
#define BOARD_UKMARSBOT_V1
// Robot variant so that the correct defaults can be selected
#define UKMARSBOT_ARIADNE



#endif