#include "sensors.h"
#include <arduino.h>
#include "analog.h"
#include "board.h"
#include "motion.h"
#include "settings.h"

volatile float gSteeringControl;
bool gSteeringEnabled;

bool gLineLock = false;
int gLineErrorDirection = 0;
static float oldError;

const float errMax = 400;

volatile float rawError = 0;
float steeringUpdate() {
  static float oldDTerm = 0;
  gSteeringControl = 0;
  float sum = getSensor(2) + getSensor(1);
  float diff = getSensor(2) - getSensor(1);
  float err = 0;
  if (!gSteeringEnabled) {
    return 0;
  }
  if (sum > 300) {
    gLineLock = true;
    err = getLineError();
    if (err > errMax) {
      err = errMax;
    }
    if (err < -errMax) {
      err = -errMax;
    }
    gLineErrorDirection = sgn(err);
  } else {
    gLineLock = false;
    err = errMax * gLineErrorDirection;
  }
  digitalWrite(13, gLineLock);
  float pTerm = settings.lineKP * err;
  float dTerm = settings.lineKD * (err - oldError);
  dTerm = 0.95*oldDTerm +0.05*dTerm;
  oldDTerm = dTerm;
  oldError = err;
  rawError = err;
  float speedAdjust = constrain(fwd.mCurrentSpeed, 500, fwd.mCurrentSpeed);
  gSteeringControl = speedAdjust * (pTerm + dTerm);
  // gSteeringControl = adjustExponential(gSteeringControl, 0.5);
  return gSteeringControl;
}

void steeringReset() {
  rawError = 0;
  oldError = 0;
  gSteeringControl = 0;
}

bool turnMarker() {
  return getSensor(3) > 250;
}

bool startMarker() {
  return getSensor(0) > 250;
}



float getLineError() {
  float sum = getSensor(2) + getSensor(1);
  float diff = getSensor(2) - getSensor(1);
   return diff/sum;
}
