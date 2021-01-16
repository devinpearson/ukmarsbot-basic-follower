#ifndef COMMANDS_H
#define COMMANDS_H

#include <arduino.h>
#include "cli.h"
void cmdFilter(float f);
void cliShowLineSensors(Args& args);
void cliShowWallSensors(Args& args);
void cliShowFront(Args& args);
void cliShowLeft(Args& args);
void cliShowRight(Args& args);
void cliShowBattery(Args& args);
void cliShowFunction(Args& args);
float cliFollowLine(Args& args);

void cliShowEncoders(Args& args);

void cliTestFwd(Args& args);  // open loop forward test
void cliTestRot(Args& args);  // open loop rotation test
void cliTestTwiddle(Args& args);  // tune the line follower

void cliTestMove(Args& args);
void cliTestTurn(Args& args);
void cliTestSpin(Args& args);
void cliLineCalibrate(Args& args);
void cliWallCalibrate(Args& args);

template <class T>
void cliSetGet(T& var, T min, T max, Args& args) {
  if (args.argc > 1) {
    var = atof(args.argv[1]);
    if (var > max) {
      var = max;
    }
    if (var < min) {
      var = min;
    }
  }
  Serial.print(args.argv[0]);
  Serial.print(F(" = "));
  Serial.println(var);
}

#endif
