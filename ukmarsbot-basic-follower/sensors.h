#ifndef SENSORS_H
#define SENSORS_H

#include <arduino.h>
extern volatile float rawError;
extern volatile float gSteeringControl;
extern bool gSteeringEnabled;
extern bool gLineLock;
extern int gLineErrorDirection;
void steeringReset();
float steeringUpdate();
float adjustExponential(float value, float factor) ;
float getLineError();

bool turnMarker();
bool startMarker();


#endif
