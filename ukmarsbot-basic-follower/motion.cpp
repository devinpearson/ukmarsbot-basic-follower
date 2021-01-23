#include "motion.h"
#include <arduino.h>
#include "defaults.h"
#include "encoders.h"
#include "motors.h"
#include "profile.h"
#include "settings.h"

bool motionEnabled;

// float fwdError;
// float rotError;

// float fwdVolts;
// float rotVolts;

float leftFF;
float rightFF;

void motionInit() {
  motionEnabled = false;
  fwd.mKP = settings.fwdKP;
  fwd.mKD = settings.fwdKD;
  rot.mKP = settings.rotKP;
  rot.mKD = settings.rotKD;
}

/** calculate the voltage to be applied to the motor for a given speed
 *  the drive train is not symmetric and there is significant stiction.
 *  If used with PID, a simpler, single value will be sufficient.
 *
 *  Multiply by (1/x) is generally more efficient than just divide by x
 */

float leftFeedForward(float speed) {
  static float oldSpeed = 0;
  leftFF = 0;
  if (speed > 0) {
    leftFF += speed * settings.leftFFSpeedFwd + settings.leftFFStaticFwd;
  } else {
    leftFF += speed * settings.leftFFSpeedRev - settings.leftFFStaticRev;
  }
  float acc = (speed - oldSpeed) * LOOP_FREQUENCY;
  oldSpeed = speed;
  leftFF += settings.leftFFAccFwd * acc;
  return leftFF;
}

float rightFeedForward(float speed) {
  static float oldSpeed = 0;
  rightFF = 0;
  if (speed > 0) {
    rightFF += speed * settings.rightFFSpeedFwd + settings.rightFFStaticFwd;
  } else {
    rightFF += speed * settings.rightFFSpeedRev - settings.rightFFStaticRev;
  }

  float acc = (speed - oldSpeed) * LOOP_FREQUENCY;
  oldSpeed = speed;
  rightFF += settings.rightFFAccFwd * acc;
  return rightFF;
}

void motionUpdate() {
  fwd.controllerUpdate();
  rot.controllerUpdate();
  float leftVolts = fwd.mControlOutput - rot.mControlOutput;
  float rightVolts = fwd.mControlOutput + rot.mControlOutput;
  if (!motionEnabled) {
    return;
  }
  float tangentSpeed = rot.mCurrentSpeed * MOUSE_RADIUS * (1 / 57.29);
  float leftSpeed = fwd.mCurrentSpeed - tangentSpeed;
  float rightSpeed = fwd.mCurrentSpeed + tangentSpeed;
  leftVolts += leftFeedForward(leftSpeed);
  rightVolts += rightFeedForward(rightSpeed);
  setLeftMotorVolts(leftVolts);
  setRightMotorVolts(rightVolts);
}

///////////////////////////////////////////////////
