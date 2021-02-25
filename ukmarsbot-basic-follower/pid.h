#ifndef PID_H
#define PID_H

#include "defaults.h"
#include <Arduino.h>

class PID {
  public:
  PID(float Kp, float Ki, float Kd, float * input, float * setpoint) : mKP(Kp),
                                      mKI(Ki),
                                      mKD(Kd),
                                      mInput(input),
                                      mSetpoint(setpoint){

                                      };

  // uses calculation describe in
  // http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
  float compute() {
    float input = *mInput;
    float error = *mSetpoint - input;
    float dInput = input - mLastInput;
    mLastInput = input;

    mOutputSum += mKI * LOOP_INTERVAL * error;

    // anti-windup
    if (mOutputSum > MAX_MOTOR_VOLTS) {
      mOutputSum = MAX_MOTOR_VOLTS;
    } else if (mOutputSum < -MAX_MOTOR_VOLTS) {
      mOutputSum = -MAX_MOTOR_VOLTS;
    }
    float output = mKP * error;
    output += mOutputSum;
    output -= mKD * LOOP_FREQUENCY * dInput;
    if (output > MAX_MOTOR_VOLTS) {
      output = MAX_MOTOR_VOLTS;
    } else if (output < -MAX_MOTOR_VOLTS) {
      output = -MAX_MOTOR_VOLTS;
    }
    output = constrain(output, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
    mOutput = output;
    // don't limit the output - that will be done elsewhere
    return output;
  }

  void set_tunings(float kp, float ki, float kd) {
    mKP = kp;
    mKI = ki;
    mKD = kd;
  }

  /* Initialize()****************************************************************
 *	does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/
  void Initialize() {
    mOutput = 0;
    mOutputSum = mOutput;
    mLastInput = *mInput;
  }

  float output() {
    return mOutput;
  }

  float mKP = 0;
  float mKI = 0;
  float mKD = 0;

  private:
  float mOutput = 0;
  float mLastInput = 0;
  float mOutputSum = 0;

  // to save RAM, these could be arguments of compute()
  float *mInput;
  float *mSetpoint;

};

#endif