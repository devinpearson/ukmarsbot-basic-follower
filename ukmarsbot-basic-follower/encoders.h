#ifndef ENCODERS_H
#define ENCODERS_H

#include <Arduino.h>
extern float encoderLeftSpeed;
extern float encoderRightSpeed;
extern float encoderPosition;
extern float encoderAngle;
extern int encoderLeftCount;
extern int encoderRightCount;
extern float encoderSpeed;
extern float encoderOmega;
extern int32_t encoderTotal;
extern int32_t encoderRotation;
void encoderReset();
void encoderSetup();
void encoderUpdate();
float getEncoderSpeed();
float getEncoderOmega();

#endif