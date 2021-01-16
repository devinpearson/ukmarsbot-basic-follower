#include "cli_commands.h"
#include <arduino.h>
#include "analog.h"
#include "cli.h"
#include "encoders.h"
#include "motion.h"
#include "motors.h"
#include "sensors.h"
#include "settings.h"
#include "streaming.h"
#include "systick.h"
#include "twiddle.h"

void cmdFilter(float f) {
  float iirOut[2];
  float angle = 2 * 3.14159 * f / 500;
  float alpha = 0.995;
  float pole[2];
  pole[0] = cos(angle) * alpha;
  pole[1] = sin(angle) * alpha;
  uint32_t t = ticks;
  while (!functionButtonPressed()) {
    while (t == ticks)
      ;
    t = ticks;
    float out[2];
    out[0] = iirOut[0] * pole[0] - iirOut[1] * pole[1] + getSensor(0);
    out[1] = iirOut[0] * pole[1] - iirOut[1] * pole[0];
    iirOut[0] = out[0];
    iirOut[1] = out[1];
    float mag = out[0] * out[0] + out[1] * out[1];
    Serial.println(mag, 4);
  }
}
void cliShowLineSensors(Args& args) {
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial << _JUSTIFY(abs(getSensor(i)), 5);
  }
  Serial.print(F("  err: "));
  Serial.print(getLineError(), 2);

  // float sum = fabsf(float(getSensor(1) + getSensor(2)));
  // float diff = float(getSensor(1) - getSensor(2));
  //   int err = sum>20?200 * (diff / sum):0;
  // Serial << _JUSTIFY(int(sum), 5);
  // Serial << _JUSTIFY(err, 5);
  Serial << endl;
}
void cliShowWallSensors(Args& args) {
  for (int i = 0; i < 3; i++) {
    Serial << _JUSTIFY(abs(getSensor(i)), 5);
  }

  // float sum = fabsf(float(getSensor(0) + getSensor(2)));
  // float diff = float(getSensor(0) - getSensor(2));
  // int err = sum>20?200 * (diff / sum):0;

  // Serial << _JUSTIFY(int(sum), 5);
  // Serial << _JUSTIFY(err, 5);
  Serial << endl;
}

void cliLineCalibrate(Args& args) {
  sensorsEnable();
  fwd.reset();
  rot.reset();
  motionEnabled = true;
  rot.startMove(360.0, 300.0, 0.0, 1000.0);
  int angle = -1;
  int a;
  while (rot.mState != Profile::FINISHED) {
    a = int(rot.mPosition);
    if (a != angle) {
      angle = a;
      Serial << _JUSTIFY(angle, 5);
      cliShowLineSensors(args);
    }
  }
  motionEnabled = false;
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  sensorsDisable();
}

void cliWallCalibrate(Args& args) {
  sensorsEnable();
  fwd.reset();
  rot.reset();
  motionEnabled = true;
  rot.startMove(360.0, 300.0, 0.0, 1000.0);
  int angle = -1;
  int a;
  while (rot.mState != Profile::FINISHED) {
    a = int(rot.mPosition);
    if (a != angle) {
      angle = a;
      Serial << _JUSTIFY(angle, 5);
      cliShowWallSensors(args);
    }
  }
  motionEnabled = false;
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  sensorsDisable();
}

void cliShowFront(Args& args) {
}

void cliShowLeft(Args& args) {
}

void cliShowRight(Args& args) {
}

void cliShowBattery(Args& args) {
}

void cliShowFunction(Args& args) {
  while(!functionButtonPressed()){
    Serial.println(functionSetting());
  }
}

void cliShowEncoders(Args& args) {
  Serial.println(F("Encoders:"));
  while (true) {
    Serial.print(F("Left = "));
    Serial.print(encoderLeftCount);
    Serial.print(F("  Right = "));
    Serial.print(encoderRightCount);
    Serial.print(F("  Tot = "));
    Serial.print(encoderTotal);
    Serial.print(F("  Rot = "));
    Serial.print(encoderRotation);
    Serial.println();
    if (functionButtonPressed()) {
      break;
    }
  }
}

/**
 * Open loop forward motion test
 * Move the robot forwards or backwards by setting the motor
 * voltage directly for a given time.
 * During motion, data is transmitted using the serial port and
 * can be collected over a Bluetooth link
 * Requires that the motor controllers be disabled.
 *
 * Command line:
 *   FWD volts time
 *
 * where time is in milliseconds
 * e.g. FWD  4.5 1000  => implies left = +4.5V, right = -4.5V, 1200ms run
 */

void cliTestFwd(Args& args) {
  uint32_t endTime = 1000;
  int volts = 3;
  if (args.argc > 1) {
    endTime = atol(args.argv[1]);
  }
  if (args.argc > 2) {
    volts = atoi(args.argv[2]);
  }
  encoderReset();
  setLeftMotorVolts(volts);
  setRightMotorVolts(volts);
  uint32_t t = millis();
  endTime += t;
  while (millis() < endTime) {
    // gather the data quickly
    float spd = encoderSpeed;
    float pos = encoderPosition;
    // send it at leisure
    Serial.print(millis() - t);
    Serial.print(' ');
    Serial.print(spd, 4);
    Serial.print(' ');
    Serial.print(pos, 4);
    Serial.println();
  }
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
}

enum { IDLE, STARTING, RUNNING, STOPPING, CROSSING };
void sendLineHeader() {
  Serial.println();
  Serial.println(F("time position Start Left Right Turn SteeringControl FastError adjust omega"));
}
void sendLineTelemetry(uint32_t t, float error) {
  int posNow = int(fwd.mPosition);
  // send it at leisure
  Serial.print(t);
  Serial.print(' ');
  Serial.print(posNow);
  Serial.print(' ');
  Serial.print(getSensor(0));
  Serial.print(' ');
  Serial.print(getSensor(1));
  Serial.print(' ');
  Serial.print(getSensor(2));
  Serial.print(' ');
  Serial.print(getSensor(3));
  Serial.print(' ');
  Serial.print(gSteeringControl);
  Serial.print(' ');
  Serial.print(error);
  Serial.print(' ');
  Serial.print(rot.mAdjustment);
  Serial.print(' ');
  Serial.print(rot.mCurrentSpeed);
  Serial.println();
}

float lineTrial() {
  encoderReset();
  sensorsEnable();
  steeringReset();
  fwd.reset();
  rot.reset();
  motionEnabled = true;
  gSteeringEnabled = true;
  gSteeringControl = 0;
  uint32_t trigger = millis();
  float errorSum = 0;
  uint32_t startTime = millis();
  fwd.startMove(500, twiddleSpeed,twiddleSpeed, 4000);
  enum {
    IDLE,
    STARTING,
    RUNNING,
    STOPPING,
  };
  int runState = STARTING;
  digitalWrite(LED_LEFT, 0);
  digitalWrite(LED_RIGHT, 0);
  float error = 0;
  float slowFilter = 0;
  float startPos = fwd.mPosition;
  float target = startPos + 100;
  while (runState != STOPPING) {
    switch (runState) {
      case STARTING:
        if (fwd.mPosition > target) {
          target += 500;
          error = 0;
          slowFilter = 0;
          errorSum = 0;
          runState = RUNNING;
        }
        break;
      case RUNNING:
        if (fwd.mPosition > target) {
          runState = STOPPING;
        }
        break;
    }
    if (millis() > trigger) {
      trigger += 5;
      if (functionButtonPressed()) {
        runState = STOPPING;
      }
      slowFilter += 0.3 * (gSteeringControl - slowFilter);
      error = fabsf(slowFilter - gSteeringControl);
      errorSum += error;
      // sendLineTelemetry(millis() - startTime, error);
    }
  }
  while (functionButtonPressed()) {
  }
  Serial.print(F("\nError Sum: "));
  Serial.print(errorSum);
  Serial.println('\n');
  // sensorsDisable();
  // setLeftMotorVolts(0);
  // setRightMotorVolts(0);
  delay(250);
  // motionEnabled = false;
  // gSteeringEnabled = false;
  return errorSum;
}

void cliTestTwiddle(Args& args) {
  motionInit();
  float* params[2] = {&settings.lineKP, &settings.lineKD};
  Serial.print(*params[0], 6);
  Serial.print("  ");
  Serial.print(*params[1], 6);
  Serial.print("  ");
  Twiddle tw(2, params, lineTrial);
  Serial.println("TWIDDLING");
  tw.go();
}

float cliFollowLine(Args& args) {
  float speed = 500;
  if (args.argc > 1) {
    speed = atof(args.argv[1]);
  }
  if (args.argc > 2) {
    settings.lineKP = atof(args.argv[2]);
  }
  if (args.argc > 3) {
    settings.lineKD = atof(args.argv[3]);
  }
  Serial.print(F("\"Line follower kD: "));
  Serial.print(settings.lineKP);
  Serial.print(F("  kD: "));
  Serial.print(settings.lineKD);
  Serial.print(F("  speed: "));
  Serial.print(speed);
  Serial.print(F("\""));
  sendLineHeader();
  encoderReset();
  steeringReset();
  sensorsEnable();
  fwd.reset();
  rot.reset();
  gSteeringControl = 0;
  gSteeringEnabled = true;
  motionEnabled = true;
  uint32_t trigger = millis();
  float errorSum = 0;
  uint32_t startTime = millis();
  fwd.startMove(500, speed, speed, 4000);

  int runState = STARTING;
  digitalWrite(LED_LEFT, 0);
  digitalWrite(LED_RIGHT, 0);
  float error = 0;
  float slowFilter = 0;
  while (runState != STOPPING) {
    uint8_t markers = startMarker() + 2 * turnMarker();
    digitalWrite(LED_RIGHT, markers & 2);
    digitalWrite(LED_LEFT, markers & 1);
    switch (runState) {
      case STARTING:
        if (markers == 3) {
          runState = CROSSING;
        }
        break;
      case CROSSING:
        if (markers == 0) {
          runState = RUNNING;
        }
        break;
      case RUNNING:
        if (markers == 3) {
          runState = STOPPING;
        }
        break;
    }
    if (fwd.mPosition > 2520) {
      runState = STOPPING;
    }
    if (fwd.mPosition > 200 && gLineLock == false) {
      runState = STOPPING;
    }

    if (millis() > trigger) {
      trigger += 5;
      if (functionButtonPressed()) {
        runState = STOPPING;
      }
      slowFilter += 0.3 * (gSteeringControl - slowFilter);
      error = fabsf(slowFilter - gSteeringControl);
      errorSum += error;
      sendLineTelemetry(millis() - startTime, rawError);
    }
  }
  while (functionButtonPressed()) {
  }
      fwd.startMove(320, speed, 0, 4000);

  // Serial.print(F("\n==========================\n\nError Sum: \""));
  // Serial.print(errorSum);
  // Serial.println(F("\"\n"));
  sensorsDisable();
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  motionEnabled = false;
  delay(250);
  gSteeringEnabled = false;
  return errorSum;
}

/**
 * Open loop rotational motion test
 * Rotates the robot left (+ve) or right (-ve) by setting the motor
 * voltage directly for a given time.
 * During motion, data is transmitted using the serial port and
 * can be collected over a Bluetoth link
 * Requires that the motor controllers be disabled.
 *
 * Command line:
 *   ROT volts time
 *
 * where time is in milliseconds
 * e.g. FWD 1200 4.5
 */

void cliTestRot(Args& args) {
  uint32_t endTime = 1500;
  int volts = 3;
  if (args.argc > 1) {
    volts = atoi(args.argv[2]);
  }
  if (args.argc > 2) {
    endTime = atol(args.argv[1]);
  }
  encoderReset();
  setLeftMotorVolts(-volts);
  setRightMotorVolts(+volts);
  uint32_t t = millis();
  endTime += t;
  while (millis() < endTime) {
    // gather the data quickly
    float spd = encoderOmega;
    float pos = encoderAngle;
    // send it at leisure
    Serial.print(millis() - t);
    Serial.print(' ');
    Serial.print(spd, 4);
    Serial.print(' ');
    Serial.print(pos, 4);
    Serial.println();
  }
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
}

void sendProfileHeader() {
  Serial.println(F("time setPos setSpeed ouput encPos encSpeed encAngle encOmega"));
}

void sendProfileData(int timeStamp, Profile& prof) {
  // gather the data quickly for consistency
  float setPos = prof.mPosition;
  float setSpeed = prof.mCurrentSpeed;
  float actPos = encoderPosition;
  float actSpeed = encoderSpeed;
  float actAngle = encoderAngle;
  float actOmega = encoderOmega;
  float control = prof.mControlOutput;
  // send it at leisure
  Serial.print(timeStamp);
  Serial.print(' ');
  Serial.print(setPos);
  Serial.print(' ');
  Serial.print(setSpeed);
  Serial.print(' ');
  Serial.print(control);
  Serial.print(' ');
  Serial.print(actPos);
  Serial.print(' ');
  Serial.print(actSpeed);
  Serial.print(' ');
  Serial.print(actAngle);
  Serial.print(' ');
  Serial.print(actOmega);
  Serial.print(' ');
  Serial.print(leftFF);
  Serial.print(' ');
  Serial.print(rightFF);
  Serial.println();
}

void cliTestMove(Args& args) {
  uint32_t t = millis();

  float dist = atof(args.argv[1]);
  float topSpeed = atof(args.argv[2]);
  float endSpeed = atof(args.argv[3]);
  float accel = atof(args.argv[4]);
  if (dist == 0) {
    dist = 400;
  }
  if (topSpeed == 0) {
    topSpeed = 400;
  }
  if (accel == 0) {
    accel = 2000;
  }
dist = 100;
topSpeed = 100;
accel = 1000;
endSpeed = 0;

  delay(500);
  sendProfileHeader();
  encoderReset();
  fwd.reset();
  rot.reset();
  motionEnabled = true;
  t = millis();
  fwd.startMove(dist, topSpeed, endSpeed, accel);
  while (fwd.mState != Profile::FINISHED) {
    sendProfileData(millis() - t, fwd);
  }

  // rot.startMove(180, 600, 0, 3000);
  // while (rot.mState != Profile::FINISHED) {
  //   sendProfileData(millis() - t, fwd);
  // }
  // fwd.startMove(500, 600, 0, 2000);
  // while (fwd.mState != Profile::FINISHED) {
  //   sendProfileData(millis() - t, fwd);
  // }

  motionEnabled = false;
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  Serial.println();
}

void cliTestSpin(Args& args) {
  uint32_t t = millis();
  float dist = atof(args.argv[1]);
  float topSpeed = atof(args.argv[2]);
  float endSpeed = atof(args.argv[3]);
  float accel = atof(args.argv[4]);
  if (dist == 0) {
    dist = 360;
  }
  if (topSpeed == 0) {
    topSpeed = 400;
  }
  if (accel == 0) {
    accel = 1500;
  }
  sendProfileHeader();
  encoderReset();
  fwd.reset();
  rot.reset();
  motionEnabled = true;
  rot.startMove(dist, topSpeed, endSpeed, accel);
  while (rot.mState != Profile::FINISHED) {
    sendProfileData(millis() - t, rot);
  }
  motionEnabled = false;
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  Serial.println();
}

void cliTestTurn(Args& args) {
  uint32_t t = millis();
  float dist = atof(args.argv[1]);
  float speed = atof(args.argv[2]);
  float omega = atof(args.argv[3]);
  float alpha = atof(args.argv[4]);
  if (dist == 0) {
    dist = 360;
  }
  if (speed == 0) {
    speed = 500;
  }
  if (omega == 0) {
    omega = 500;
  }
  if (alpha == 0) {
    alpha = 5000;
  }
  sendProfileHeader();
  gSteeringEnabled = false;
  encoderReset();
  fwd.reset();
  rot.reset();
  motionEnabled = true;

  fwd.startMove(300, speed, speed, 2500);
  while (!fwd.isFinished()) {
  }

  rot.startMove(dist, omega, 0, alpha);
  while (rot.mState != Profile::FINISHED) {
    sendProfileData(millis() - t, rot);
  }

  fwd.startMove(300, speed, 0, 2500);
  while (!fwd.isFinished()) {
  }

  motionEnabled = false;
  setLeftMotorVolts(0);
  setRightMotorVolts(0);
  Serial.println();
}
