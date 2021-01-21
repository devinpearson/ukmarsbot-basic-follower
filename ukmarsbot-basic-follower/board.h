#ifndef BOARD_H
#define BOARD_H

#include "config.h"

#ifdef BOARD_UKMARSBOT_V1
/**
 * Hardware pin defines
 */
const int ENCODER_LEFT_CLK = 2;
const int ENCODER_RIGHT_CLK = 3;
const int ENCODER_LEFT_B = 4;
const int ENCODER_RIGHT_B = 5;
const int USER_IO = 6;
const int LED_RIGHT = 6;
const int MOTOR_LEFT_DIR = 7;
const int MOTOR_RIGHT_DIR = 8;
const int MOTOR_LEFT_PWM = 9;
const int MOTOR_RIGHT_PWM = 10;
const int LED_LEFT = 11;
const int EMITTER_A = 11;
const int EMITTER_B = 12;

const int SENSOR_0 = A0;
const int SENSOR_1 = A1;
const int SENSOR_2 = A2;
const int SENSOR_3 = A3;
const int SENSOR_4 = A4;
const int SENSOR_5 = A5;
// names for the basic wall sensor channels
const int RIGHT_WALL_SENSOR = A0;
const int FRONT_WALL_SENSOR = A1;
const int LEFT_WALL_SENSOR = A2;

const int FUNCTION = A6;
const int BATTERY_VOLTS = A7;
#else
#error "UNKOWN BOARD TYPE"
#endif

extern volatile float batteryVolts;

#endif