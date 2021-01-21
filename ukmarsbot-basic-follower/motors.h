#ifndef MOTORS_H
#define MOTORS_H

#include <arduino.h>
#include "sensors.h"
enum { PWM_488_HZ, PWM_3900_HZ, PWM_31_KHZ };

void setLeftMotorPWM(int pwm);
void setRightMotorPWM(int pwm);
void setLeftMotorVolts(float volts);
void setRightMotorVolts(float volts);
void pwmSetup(int frequency);
void motorSetup();
void motorBeep(int freq);
void motorsStop();
#endif
