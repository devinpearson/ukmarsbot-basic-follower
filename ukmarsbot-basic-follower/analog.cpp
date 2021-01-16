/***********************************************************************
 * Created by Peter Harrison on 2019-09-27.
 * Copyright (c) 2019 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "analog.h"
#include <arduino.h>
#include "board.h"
#include "motion.h"
#include "sensors.h"
#include "settings.h"
volatile int sensor[SENSOR_COUNT];
volatile float batteryVolts;
volatile int functionValue;
static volatile bool sensorsEnabled = false;

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
  if (functionValue > 500) { // SW1
    return 1024 - abs(sensor[i]);
  } else {
    return abs(sensor[i]);
  }
}
void sensorsSetup() {
  // increase speed of ADC conversions to 28us each
  // by changing the clock prescaler from 128 to 16
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
  sensorsDisable();
}  // basic sensor update takes about 250us with ADC clock prescaler of 16
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

int functionSetting() {
//  values for all function settings
//  {44,  127, 210, 267, 340, 382, 423, 456, 516, 541, 566, 583, 609, 625, 640, 655, 700};
  const int thresholds[] = {85, 168, 238, 303, 361, 402, 439, 486, 528, 553, 574, 596, 617, 632, 647, 677};
  for (int i = 0; i <= 15; i++) {
    if(functionValue<thresholds[i]){
      return i;
    }
  }
  return 16;
}