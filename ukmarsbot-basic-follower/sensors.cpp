#include "sensors.h"
#include <arduino.h>
#include "board.h"
#include "motion.h"
#include "settings.h"

volatile int sensor[SENSOR_COUNT];
volatile float batteryVolts;
volatile int functionValue;
volatile float gSteeringControl;
bool gSteeringEnabled;

bool gLineLock = false;
int gLineErrorDirection = 0;
static volatile bool sensorsEnabled = false;
static float oldError;

const float errMax = 400;

/******************************************
 * Fast exponential functions
 *
 * Copyright (C) 2012 Paul Mineiro
 * All rights reserved.
 */
static inline float fasterpow2(float p) {
  float clipp = (p < -126.0f) ? -126.0f : p;
  union {
    uint32_t i;
    float f;
  } v = {uint32_t((1UL << 23) * (clipp + 126.94269504f))};
  return v.f;
}

static inline float fasterexp(float p) {
  return fasterpow2(1.442695040f * p);
}

/****************************************/

float adjustExponential(float value, float factor) {
  value /= errMax;
  value = value * fasterexp(abs(factor * value)) / fasterexp(factor);
  value *= errMax;
  return value;
}
volatile float rawError = 0;
float steeringUpdate() {
  gSteeringControl = 0;
  float sum = getSensor(2) + getSensor(1);
  float diff = getSensor(2) - getSensor(1);
  float err = 0;
  if (!gSteeringEnabled) {
    return 0;
  }
  if (sum > 300) {
    gLineLock = true;
    err = diff / sum;
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
  oldError = err;
  rawError = err;
  float speedAdjust = constrain(fwd.mCurrentSpeed, 500, fwd.mCurrentSpeed);
  gSteeringControl = speedAdjust * (pTerm + dTerm);
  gSteeringControl = adjustExponential(gSteeringControl, 0.5);
  return gSteeringControl;
}

void steeringReset() {
  rawError = 0;
  oldError = 0;
  gSteeringControl = 0;
}
void sensorsEnable() {
  sensorsEnabled = true;
}

void sensorsDisable() {
  sensorsEnabled = false;
}

int getSensor(uint8_t i) {
  if (i >= SENSOR_COUNT) {
    return 0;
  }
  return abs(sensor[i]);
}

void sensorsSetup() {
  // increase speed of ADC conversions to 28us each
  // by changing the clock prescaler from 128 to 16
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
  sensorsDisable();
}

// basic sensor update takes about 250us with ADC clock prescaler of 16
void sensorsUpdate() {
  int tmp[SENSOR_COUNT];
  batteryVolts = analogRead(BATTERY_VOLTS) * (1 / 102.4);
  functionValue = analogRead(FUNCTION);
  if (!sensorsEnabled) {
    for (int i = 0; i < SENSOR_COUNT; i++) {
      sensor[i] = 0;
    }
    return;
  }
  // read all the channels with no emitter
  for (int i = 0; i < SENSOR_COUNT; i++) {
    tmp[i] = analogRead(A0 + i);
  }
  digitalWrite(EMITTER_B, 1);
  // allow the detector response to rise close to final value
  delayMicroseconds(DEFAULTS_EMITTER_ON_TIME);
  // read only the wall/line sensor differences
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensor[i] = analogRead(A0 + i) - tmp[i];
  }
  digitalWrite(EMITTER_B, 0);
}

bool turnMarker() {
  return getSensor(3) > 250;
}

bool startMarker() {
  return getSensor(0) > 250;
}
