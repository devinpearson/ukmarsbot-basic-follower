#ifndef SENSORS_H
#define SENSORS_H

#include <arduino.h>
extern volatile float batteryVolts;
extern volatile int functionValue;

extern volatile float gSteeringControl;
extern bool gSteeringEnabled;
void steeringReset();
float steeringUpdate();
float adjustExponential(float value, float factor);
inline bool functionButtonPressed() {
  return functionValue > 750;
}
inline void waitForButtonClick() {
  while (not(functionButtonPressed())) {
  };  // do nothing
  while (functionButtonPressed()) {
  };  // do nothing
}

/*** wall sensor variables ***/
extern volatile int gSensorFrontWall;
extern volatile int gSensorLeftWall;
extern volatile int gSensorRightWall;
// true if a wall is present
extern volatile bool gLeftWall;
extern volatile bool gFrontWall;
extern volatile bool gRightWall;
extern volatile float gSensorFrontError;  // zero when robot in cell centre

/*** line sensor variables ***/
extern volatile float gSensorStartMarker;
extern volatile float gSensorTurnMarker;
extern volatile float gSensorRight;
extern volatile float gSensorLeft;

/*** steering variables ***/
extern volatile float gSensorCTE;

bool turnMarker();
bool startMarker();

void sensorsEnable();
void sensorsDisable();
int getSensor(uint8_t i);
void sensorsShow();

void sensorsSetup();
void sensorsUpdate();

float lineSensorUpdate();
void lineSensorShow();

float wallSensorUpdate();
void wallSensorShow();

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

#endif
