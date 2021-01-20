#ifndef SENSORS_H
#define SENSORS_H

#include <arduino.h>
extern volatile float batteryVolts;
extern volatile int functionValue;
extern volatile float rawError;
extern volatile float gSteeringControl;
extern bool gSteeringEnabled;
extern bool gLineLock;
extern int gLineErrorDirection;
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
bool turnMarker();
bool startMarker();

void sensorsEnable();
void sensorsDisable();
int getSensor(uint8_t i);
void sensorsSetup();
void sensorsUpdate();
template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

#endif
