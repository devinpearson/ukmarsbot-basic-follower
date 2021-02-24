#ifndef BLINKER_H
#define BLINKER_H
#include "digitalWriteFast.h"
#include <Arduino.h>

class Blinker {
  public:
  const bool NOW = true;
  explicit Blinker(uint8_t pin) : mPin(pin) { pinMode(mPin, OUTPUT); };

  Blinker &setPeriod(uint32_t ms) {
    mPeriod = ms;
    mTickTime = millis() + mPeriod / 2;
    update(NOW);
    return static_cast<Blinker &>(*this);
  }

  Blinker &setDuty(uint8_t duty) {
    if (duty > 100) {
      duty = 100;
    }
    mDuty = duty;
    update(NOW);
    return static_cast<Blinker &>(*this);
  }

  uint8_t getDuty() { return mDuty; }

  Blinker &on() {
    setDuty(100);
    return static_cast<Blinker &>(*this);
  }

  Blinker &off(uint32_t ms) {
    setDuty(0);
    return static_cast<Blinker &>(*this);
  }

  Blinker &update(bool now = false) {
    if (now || millis() > mTickTime) {
      if (mState == 1) {
        mState = 0;
        mTickTime += (mPeriod * (100 - mDuty)) / 100;
      } else {
        mState = 1;
        mTickTime += (mPeriod * mDuty) / 100;
      }
      if (mDuty == 0) {
        digitalWriteFast(mPin, 0);
      } else if (mDuty == 100) {
        digitalWriteFast(mPin, 1);
      } else {
        digitalWriteFast(mPin, mState);
      }
    }
    return static_cast<Blinker &>(*this);
  }

  uint8_t mPin;
  uint32_t mPeriod = 1000;
  int8_t mDuty = 20;
  int8_t mState;
  uint32_t mTickTime;
};

#endif
