#ifndef COMMANDS_H
#define COMMANDS_H

#include "cli.h"
#include <arduino.h>
void cmdFilter(float f);
void cmdShowFront(Args &args);
void cmdShowLeft(Args &args);
void cmdShowRight(Args &args);
void cmdShowBattery(Args &args);
void cmdShowFunction(Args &args);
float cmdFollowLine(Args &args);

void cmdShowEncoders(Args &args);
void cmdTestMotors(Args &args);

void cmdTestFwd(Args &args);     // open loop forward test
void cmdTestRot(Args &args);     // open loop rotation test
void cmdTestTwiddle(Args &args); // tune the line follower

void cmdTestMove(Args &args);
void cmdTestTurn(Args &args);
void cmdTestSpin(Args &args);
void cmdLineCalibrate(Args &args);
void cmdWallCalibrate(Args &args);

void cmdFollowWall();
void cmdSearch();
void spin(const float angle);

void turn(const int direction);
void goHalfCell(const bool stopAtEnd);

template <class T>
T cmdSetGet(T &var, Args &args) {
  if (args.argc > 1) {
    var = atof(args.argv[1]);
  }
  Serial.print(args.argv[0]);
  Serial.print(F(" = "));
  Serial.println(var);
  return var;
}

#endif
