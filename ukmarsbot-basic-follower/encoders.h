#ifndef ENCODERS_H
#define ENCODERS_H

#include <Arduino.h>
extern float encoderLeftSpeed;
extern float encoderRightSpeed;
extern float encoderPosition;
extern int32_t encoderTotal;
extern int32_t encoderRotation;
extern float encoderAngle;
extern int encoderLeftCount;
extern int encoderRightCount;
extern float encoderSpeed;
extern float encoderOmega;
void encoderReset();
void encoderSetup();
void encoderUpdate();
float getEncoderSpeed();
float getEncoderOmega();


#endif