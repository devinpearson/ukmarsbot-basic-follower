
#include "profile.h"
#include "defaults.h"
#include "encoders.h"
#include "motors.h"
#include "sensors.h"
#include "settings.h"
#include <arduino.h>

/***
 * Speeds are used for feedback because the position is changed at the start
 * of each profile
 */
Profile fwd(&encoderSpeed);
Profile rot(&encoderOmega);

void Profile::reset() {
  noInterrupts();
  mCurrentSpeed = 0;
  mTargetSpeed = 0;
  mPosition = 0;
  mState = FINISHED;
  interrupts();
}

void Profile::start_move(float distance, float topSpeed, float endSpeed, float acceleration) {
  int8_t sign = 1;
  if (distance < 0) {
    distance = -distance;
    sign = -1;
  }
  if (distance < 1.0) {
    mState = FINISHED;
    return;
  }
  if (endSpeed > topSpeed) {
    endSpeed = topSpeed;
  }
  noInterrupts();
  mDirection = sign;
  mPosition = 0;
  mEndPosition = distance;
  mTargetSpeed = sign * fabsf(topSpeed);
  mEndSpeed = sign * fabsf(endSpeed);
  mAcceleration = fabsf(acceleration);
  mState = ACCELERATE;
  interrupts();
}

void Profile::make_move(float distance, float topSpeed, float endSpeed, float acceleration) {
  start_move(distance, topSpeed, endSpeed, acceleration);
  while (!is_finished()) {
    // do nothing
  }
}

bool Profile::is_finished() {
  return mState == FINISHED;
}

float Profile::get_braking_acceleration(float distance, float speed, float endspeed) {
  speed = fabsf(speed);
  endspeed = fabsf(endspeed);
  if (endspeed > speed) {
    return 0;
  }
  distance = fabsf(distance);
  float result = ((speed * speed) - (endspeed * endspeed)) / (2 * distance);
  return result;
}

void Profile::adjust(float adjustment) {
  if (motor_controllers_enabled) {
    mCurrentSpeed += adjustment;
  }
}

//TODO: Time this method!
void Profile::update() {
  if (mState == ACCELERATE) {
    float deceleration = get_braking_acceleration(fabsf(mEndPosition) - fabsf(mPosition), mCurrentSpeed, mEndSpeed);
    if (deceleration >= mAcceleration) {
      mState = BRAKE;
      mTargetSpeed = mEndSpeed;
      if (fabsf(mEndSpeed) < mAcceleration * LOOP_INTERVAL) {
        mTargetSpeed = mDirection * mAcceleration * LOOP_INTERVAL;
      }
    }
  }
  /////////
  if (mCurrentSpeed < mTargetSpeed) {
    mCurrentSpeed += mAcceleration * LOOP_INTERVAL;
    if (mCurrentSpeed > mTargetSpeed) {
      mCurrentSpeed = mTargetSpeed;
    }
  } else if (mCurrentSpeed > mTargetSpeed) {
    mCurrentSpeed -= mAcceleration * LOOP_INTERVAL;
    if (mCurrentSpeed < mTargetSpeed) {
      mCurrentSpeed = mTargetSpeed;
    }
  }
  ///////////
  mPosition += (mCurrentSpeed)*LOOP_INTERVAL;
  ///////////
  if (mState == BRAKE) {
    if (fabsf(mCurrentSpeed) - fabsf(mTargetSpeed) < 0.1) {
      mState = FINISHED;
      mCurrentSpeed = mEndSpeed;
    }
    if ((fabsf(mPosition)) >= fabsf(mEndPosition)) {
      mState = FINISHED;
      mCurrentSpeed = mEndSpeed;
    }
  }
}


