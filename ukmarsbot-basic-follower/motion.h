#ifndef MOTION_H
#define MOTION_H
#include "board.h"
#include "sensors.h"
/***
 * fwdSpeed is in mm/s
 * turnSpeed is in deg/s
 */

extern float leftFF;
extern float rightFF;

// Add constructor that tells profile where its feedback comes from.

extern bool motionEnabled;

void motionUpdate();
void motionInit();

#endif
