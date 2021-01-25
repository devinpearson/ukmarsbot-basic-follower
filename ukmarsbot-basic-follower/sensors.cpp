#include "sensors.h"
#include <arduino.h>
#include "board.h"
#include "digitalWriteFast.h"
#include "motion.h"
#include "profile.h"
#include "settings.h"
#include "streaming.h"

volatile int sensor[SENSOR_COUNT];
volatile float batteryVolts;
volatile int functionValue;
volatile float gSteeringControl;
bool gSteeringEnabled;

static volatile bool sensorsEnabled = false;
static float fTerm;  // used to filter the D term in the steering controller
static float oldError = 0;

const float errMax = 400;

/*** wall sensor variables ***/
volatile int gSensorFrontWall;
volatile int gSensorLeftWall;
volatile int gSensorRightWall;
// true if a wall is present
volatile bool gLeftWall;
volatile bool gFrontWall;
volatile bool gRightWall;
volatile float gSensorFrontError;  // zero when robot in cell centre

/*** line sensor variables ***/
// although the values need not be global variables, it is helpful
// when debugging to be able to print them out
volatile float gSensorStartMarker;
volatile float gSensorTurnMarker;
volatile float gSensorRight;
volatile float gSensorLeft;
volatile float gSensorSum;
volatile float gSensorDifference;
static int gLineErrorDirection = 0;

/*** steering variables ***/
volatile float gSensorCTE;

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

float steeringUpdate() {
  float err = 0;
  float pTerm = 0;
  float dTerm = 0;
  float kP = 0;
  float kD = 0;
  // always calculate the errors even if not used so that debugging is possible
  switch (settings.mode) {
    case MODE_LINE: {
      err = lineSensorUpdate();
      kP = settings.lineKP;
      kD = settings.lineKD;
    } break;
    case MODE_MAZE: {
      err = wallSensorUpdate();
      kP = settings.wallKP;
      kD = settings.wallKD;
    } break;
    case MODE_NONE:
    default: {
      err = 0;
    } break;
  }

  if (!gSteeringEnabled) {
    gSteeringControl = 0;
    return 0;
  }
  pTerm = kP * err;
  dTerm = kD * (err - oldError);
  oldError = err;
  pTerm = settings.wallKP * err;
  fTerm = 0.1 * (dTerm) + 0.9 * fTerm;

  float speedAdjust = constrain(fwd.mCurrentSpeed, 800, fwd.mCurrentSpeed);
  gSteeringControl = speedAdjust * (pTerm + fTerm);
  gSteeringControl = constrain(gSteeringControl, -5, 5);
  // gSteeringControl = adjustExponential(gSteeringControl, 0.5);
  return gSteeringControl;
}

void steeringReset() {
  oldError = 0;
  fTerm = 0;
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
// reads all sensor chanels into an array for processing as needed
// sensors are always read with a pulsed emitter
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
  digitalWriteFast(EMITTER_B, 1);
  // allow the detector response to rise close to final value
  delayMicroseconds(DEFAULTS_EMITTER_ON_TIME);
  // read only the wall/line sensor differences
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensor[i] = analogRead(A0 + i) - tmp[i];
  }
  digitalWriteFast(EMITTER_B, 0);
}

void sensorsShow() {
  for (int i = 0; i < 5; i++) {
    Serial << _JUSTIFY(abs(getSensor(i)), 5);
  }
  Serial << endl;
}

/*********************************** Line tracking **************************/
float lineSensorUpdate() {
  if (settings.mode != MODE_LINE) {
    return;
  }
  gSensorStartMarker = sensor[0];
  gSensorRight = sensor[1];
  gSensorLeft = sensor[2];
  gSensorTurnMarker = sensor[3];
  if (gSensorStartMarker > DEFAULTS_RIGHT_MARKER_THRESHOLD) {
    digitalWrite(LED_RIGHT, 1);
  } else {
    digitalWrite(LED_RIGHT, 0);
  }
  if (gSensorTurnMarker > DEFAULTS_LEFT_MARKER_THRESHOLD) {
    digitalWrite(LED_LEFT, 1);
  } else {
    digitalWrite(LED_LEFT, 0);
  }
  gSensorSum = gSensorRight + gSensorLeft;
  gSensorDifference = gSensorRight - gSensorLeft;
  if (gSensorSum > DEFAULTS_LINE_DETECT_THRESHOLD) {
    gSensorCTE = DEFAULTS_LINE_WIDTH * (gSensorDifference / gSensorSum);
  } else {
    gSensorCTE = 0;
  }
  return gSensorCTE;
}

bool turnMarker() {
  return getSensor(3) > 250;
}

bool startMarker() {
  return getSensor(0) > 250;
}

void lineSensorShow() {
  Serial << _JUSTIFY(gSensorLeft, 5);
  Serial << _JUSTIFY(gSensorRight, 5);
  Serial << _JUSTIFY(gSensorCTE, 5);
  Serial << _JUSTIFY(gSensorStartMarker, 5);
  Serial << _JUSTIFY(gSensorTurnMarker, 5);
  Serial << endl;
}

/*********************************** Wall tracking **************************/
float wallSensorUpdate() {
  if (settings.mode != MODE_MAZE) {
    return;
  }
  // a single floating point multiply is MUCH less expensive than using
  // a 32 bit multiply followed by a divide!
  gSensorLeftWall = sensor[2] * LEFT_ADJUST;
  gSensorFrontWall = sensor[1] * FRONT_ADJUST;
  gSensorRightWall = sensor[0] * RIGHT_ADJUST;
  if (gLeftWall) {
    gLeftWall = gSensorLeftWall > LEFT_WALL_THRESHOLD;
  } else {
    gLeftWall = gSensorLeftWall > (LEFT_WALL_THRESHOLD + 20);
  }
  if (gRightWall) {
    gRightWall = gSensorRightWall > RIGHT_WALL_THRESHOLD;
  } else {
    gRightWall = gSensorRightWall > (RIGHT_WALL_THRESHOLD + 20);
  }
  gFrontWall = gSensorFrontWall > FRONT_WALL_THRESHOLD;
  digitalWriteFast(LED_LEFT, gLeftWall);
  digitalWriteFast(LED_RIGHT, gRightWall);
  digitalWriteFast(LED_BUILTIN, gFrontWall);
  // calculate the alignment errors - too far right is negative
  int error = 0;
  if ((gSensorLeftWall + gSensorRightWall) > 90) {
    if (gSensorLeftWall > gSensorRightWall) {
      error = (LEFT_NOMINAL - gSensorLeftWall);
    } else {
      error = (gSensorRightWall - RIGHT_NOMINAL);
    }
  }
  if (gSensorFrontWall > 100) {
    error = 0;
  }
  gSensorCTE = DEFAULTS_WALL_CTE_GAIN * error;
  gSensorFrontError = FRONT_NOMINAL - gSensorFrontWall;  // Too close is negative
  return gSensorCTE;
}

void wallSensorShow() {
  Serial << _JUSTIFY(gSensorLeftWall, 5);
  Serial << _JUSTIFY(gSensorFrontWall, 5);
  Serial << _JUSTIFY(gSensorRightWall, 5);
  Serial << _JUSTIFY(gSensorCTE, 5);
  Serial << _JUSTIFY(gSensorFrontError, 5);
  Serial << endl;
}