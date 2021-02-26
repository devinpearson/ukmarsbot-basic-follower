
#include "blinker.h"
#include "board.h"
#include "cli.h"
#include "commands.h"
#include "encoders.h"
#include "motors.h"
#include "profile.h"
#include "sensors.h"
#include "settings.h"
#include "stopwatch.h"
#include "systick.h"
#include <Arduino.h>

// overload the standard map() to get floating point results
float map(long x, long in_min, long in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t updateInterval = 10; // milliseconds

/****************************************************************************/


void setup() {
  Serial.begin(BAUDRATE);
  settingsRead();

  Serial.println(settings.fwdKP, 5);
  Serial.println(settings.fwdKD, 5);
  Serial.println(settings.mmPerCount, 5);
  Serial.println(settings.degPerCount, 5);
  Serial.println(settings.mode, 5);
  Serial.println(settings.wallKP, 5);
  Serial.println(settings.wallKD, 5);
  Serial.println(motor_battery_comp);

  setup_systick();
  pinMode(USER_IO, OUTPUT);
  pinMode(EMITTER_A, OUTPUT);
  pinMode(EMITTER_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  setup_motors();
  encoderSetup();
  sensorsSetup();
  setup_motor_controllers();
  Serial.println(F("RDY"));
  cliPrompt();
}

void execute() {
  Args args = cliSplit(); // can take 30-80 ms
  if (args.argc == 0) {
    return;
  }
  // Scanning the list may take 300 ms. More if further items aere added
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
    cmdSetGet(settings.fudge,args);
    } else if (strcmp_P(args.argv[0], PSTR("FWDKP")) == 0) {
      cmdSetGet(fwd_controller.mKP,args);
    } else if (strcmp_P(args.argv[0], PSTR("FWDKI")) == 0) {
      cmdSetGet(fwd_controller.mKI, args);
    } else if (strcmp_P(args.argv[0], PSTR("FWDKD")) == 0) {
      cmdSetGet(fwd_controller.mKD, args);
    } else if (strcmp_P(args.argv[0], PSTR("ROTKP")) == 0) {
      cmdSetGet(rot_controller.mKP,args);
    } else if (strcmp_P(args.argv[0], PSTR("ROTKI")) == 0) {
      cmdSetGet(rot_controller.mKI, args);
    } else if (strcmp_P(args.argv[0], PSTR("ROTKD")) == 0) {
      cmdSetGet(rot_controller.mKD, args);
  } else if (strcmp_P(args.argv[0], PSTR("LINEKP")) == 0) {
    cmdSetGet(settings.lineKP, args);
  } else if (strcmp_P(args.argv[0], PSTR("LINEKD")) == 0) {
    cmdSetGet(settings.lineKD, args);
  } else if (strcmp_P(args.argv[0], PSTR("WALLKP")) == 0) {
    cmdSetGet(settings.wallKP,args);
  } else if (strcmp_P(args.argv[0], PSTR("WALLKD")) == 0) {
    cmdSetGet(settings.wallKD, args);
  } else if (strcmp_P(args.argv[0], PSTR("BATT")) == 0) {
    Serial.print(batteryVolts, 2);
    Serial.println(F(" Volts"));
  } else if (strcmp_P(args.argv[0], PSTR("ENC")) == 0) {
    cmdShowEncoders(args);
  } else if (strcmp_P(args.argv[0], PSTR("PWM")) == 0) {
    cmdTestMotors(args);
  } else if (strcmp_P(args.argv[0], PSTR("ROT")) == 0) {
    cmdTestRot(args);
  } else if (strcmp_P(args.argv[0], PSTR("FWD")) == 0) {
    cmdTestFwd(args);
  } else if (strcmp_P(args.argv[0], PSTR("MOVE")) == 0) {
    cmdTestMove(args);
  } else if (strcmp_P(args.argv[0], PSTR("TURN")) == 0) {
    cmdTestTurn(args);
  } else if (strcmp_P(args.argv[0], PSTR("SPIN")) == 0) {
    cmdTestSpin(args);
  } else if (strcmp_P(args.argv[0], PSTR("LINECAL")) == 0) {
    cmdLineCalibrate(args);
  } else if (strcmp_P(args.argv[0], PSTR("WALLCAL")) == 0) {
    cmdWallCalibrate(args);
  } else if (strcmp_P(args.argv[0], PSTR("FOLLOW")) == 0) {
    cmdFollowLine(args);
  } else if (strcmp_P(args.argv[0], PSTR("TWIDDLE")) == 0) {
    cmdTestTwiddle(args);
  } else if (strcmp_P(args.argv[0], PSTR("BRAKE")) == 0) {
    float dist = atof(args.argv[1]);
    float speed = atof(args.argv[2]);
    float endspeed = atof(args.argv[3]);
    float deceleration = fwd.get_braking_acceleration(dist, speed, endspeed);
    Serial.println(deceleration, 4);
  } else if (strcmp_P(args.argv[0], PSTR("LINE")) == 0) {
    sensorsEnable();
    while (!functionButtonPressed()) {
      lineSensorShow();
      delay(50);
    }
    sensorsDisable();
  } else if (strcmp_P(args.argv[0], PSTR("WALL")) == 0) {
    sensorsEnable();
    while (!functionButtonPressed()) {
      wallSensorShow();
      delay(50);
    }
    sensorsDisable();
  } else if (strcmp_P(args.argv[0], PSTR("SENSORS")) == 0) {
    sensorsEnable();
    while (!functionButtonPressed()) {
      sensorsShow();
      delay(50);
    }
    sensorsDisable();
  } else if (strcmp_P(args.argv[0], PSTR("SEARCH")) == 0) {
    cmdFollowWall();
  } else {
    Serial.println("ERR");
  }
}

void loop() {
  if (getLine()) {
    execute();
    clearLine();
    cliPrompt();
  }
}
