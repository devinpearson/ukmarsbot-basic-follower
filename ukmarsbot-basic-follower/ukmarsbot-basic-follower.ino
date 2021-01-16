
#include <Arduino.h>
// #include "PID_v1.h"
#include "analog.h"
#include "blinker.h"
#include "board.h"
#include "cli.h"
#include "cli_commands.h"
#include "encoders.h"
#include "motion.h"
#include "motors.h"
#include "sensors.h"
#include "settings.h"
#include "systick.h"
#include "twiddle.h"

Blinker blinker = Blinker(LED_BUILTIN).setPeriod(1000).setDuty(50);

// replace the standard map() to get floating point results
float map(long x, long in_min, long in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/****************************************************************************/

uint8_t checkReset() {
  uint8_t status = MCUSR;
  if (status & 1) {
    Serial.println(F("Power on reset"));
  }
  if (status & 2) {
    Serial.println(F("External Reset"));  // reset button
  }
  if (status & 4) {
    Serial.println(F("Brownout reset"));
  }
  if (status & 8) {
    Serial.println(F("Watchdog reset"));
  }
  if (status & 32) {
    Serial.println(F("Software reset"));
  }
  MCUSR = 0;
  return status;
}

void setup() {
  Serial.begin(BAUDRATE);
  checkReset();
  settingsRead();

  Serial.println(settings.leftFFSpeedFwd, 5);
  Serial.println(settings.rightFFSpeedFwd, 5);
  Serial.println(settings.leftFFStaticFwd, 5);
  Serial.println(settings.rightFFStaticFwd, 5);
  Serial.println(DEFAULTS_COUNTS_PER_MM);
  Serial.println(DEFAULTS_COUNTS_PER_DEG);
  setupSystick(1000);
  pinMode(USER_IO, OUTPUT);
  pinMode(EMITTER_A, OUTPUT);
  pinMode(EMITTER_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  motorSetup();
  encoderSetup();
  sensorsSetup();
  motionInit();
  Serial.println(F("RDY"));
  cliPrompt();
}

void execute() {
  Args args = cliSplit();
  if (strcmp_P(args.argv[0], PSTR("WRITE")) == 0) {
    settingsWrite();
    Serial.println(F("OK - Settings written to EEPROM"));
  } else if (strcmp_P(args.argv[0], PSTR("READ")) == 0) {
    settingsRead();
    Serial.println(F("OK - Settings read from EEPROM, changes lost"));
  } else if (strcmp_P(args.argv[0], PSTR("CLEAR")) == 0) {
    settings = defaults;
    Serial.println(F("OK - Settings cleared to defaults"));
  } else if (strcmp_P(args.argv[0], PSTR("RESET")) == 0) {
    settingsReset();
    Serial.println(F("OK - Settings cleared to defaults and saved to EEPROM"));
  } else if (strcmp_P(args.argv[0], PSTR("FUDGE")) == 0) {
    cliSetGet(settings.fudge, 0.0f, 10.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("FWDKP")) == 0) {
    cliSetGet(fwd.mKP, 0.0f, 10.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("FWDKD")) == 0) {
    cliSetGet(fwd.mKD, 0.0f, 10.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("ROTKP")) == 0) {
    cliSetGet(rot.mKP, 0.0f, 10.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("ROTKD")) == 0) {
    cliSetGet(rot.mKD, 0.0f, 10.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("LINEKP")) == 0) {
    cliSetGet(settings.lineKP, 0.0f, 2000.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("LINEKD")) == 0) {
    cliSetGet(settings.lineKD, 0.0f, 2000.0f, args);
  } else if (strcmp_P(args.argv[0], PSTR("BATT")) == 0) {
    Serial.print(batteryVolts, 2);
    Serial.println(F(" Volts"));
  } else if (strcmp_P(args.argv[0], PSTR("FUNC")) == 0) {
    cliShowFunction(args);
  } else if (strcmp_P(args.argv[0], PSTR("BEEP")) == 0) {
    motorBeep(500);
    motorBeep(1000);
  } else if (strcmp_P(args.argv[0], PSTR("ENC")) == 0) {
    cliShowEncoders(args);
  } else if (strcmp_P(args.argv[0], PSTR("BLINK")) == 0) {
    cliSetGet(blinker.mDuty, int8_t(0), int8_t(100), args);
  } else if (strcmp_P(args.argv[0], PSTR("ROT")) == 0) {
    cliTestRot(args);
  } else if (strcmp_P(args.argv[0], PSTR("FWD")) == 0) {
    cliTestFwd(args);
  } else if (strcmp_P(args.argv[0], PSTR("MOVE")) == 0) {
    cliTestMove(args);
  } else if (strcmp_P(args.argv[0], PSTR("TURN")) == 0) {
    cliTestTurn(args);
  } else if (strcmp_P(args.argv[0], PSTR("SPIN")) == 0) {
    cliTestSpin(args);
  } else if (strcmp_P(args.argv[0], PSTR("LINECAL")) == 0) {
    cliLineCalibrate(args);
  } else if (strcmp_P(args.argv[0], PSTR("WALLCAL")) == 0) {
    cliWallCalibrate(args);
  } else if (strcmp_P(args.argv[0], PSTR("FOLLOW")) == 0) {
    cliFollowLine(args);
  } else if (strcmp_P(args.argv[0], PSTR("TWIDDLE")) == 0) {
    twiddleSpeed = 500;
    if (args.argc > 1) {
      twiddleSpeed = atof(args.argv[1]);
    }
    cliTestTwiddle(args);
  } else if (strcmp_P(args.argv[0], PSTR("BRAKE")) == 0) {
    float dist = atof(args.argv[1]);
    float speed = atof(args.argv[2]);
    float endspeed = atof(args.argv[3]);
    float deceleration = fwd.getBraking(dist, speed, endspeed);
    Serial.println(deceleration, 4);

  } else if (strcmp_P(args.argv[0], PSTR("LINE")) == 0) {
    sensorsEnable();
    while (!functionButtonPressed()) {
      cliShowLineSensors(args);
      delay(50);
    }
    sensorsDisable();
  } else if (strcmp_P(args.argv[0], PSTR("WALL")) == 0) {
    sensorsEnable();
    while (!functionButtonPressed()) {
      cliShowWallSensors(args);
      delay(50);
    }
    sensorsDisable();
  }
}

void loop() {
  blinker.update();
  // setLeftMotorVolts(0);
  // setRightMotorVolts(0);
  if (getLine()) {
    execute();
    clearLine();
    cliPrompt();
  }
}
