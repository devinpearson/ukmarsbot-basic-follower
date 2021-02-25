#ifndef MOTORS_H
#define MOTORS_H

#include "pid.h"
#include <arduino.h>
/***
 * The motors module provides low  control of the drive motors
 * in a two-wheel differential drive robot.
 */

extern PID fwd_controller;
extern PID rot_controller;

extern bool motor_controllers_enabled;
extern bool feedforward_enabled;
extern volatile float left_motor_volts;
extern volatile float right_motor_volts;

extern float motor_battery_comp;
void update_motor_controllers();
void setup_motor_controllers();

enum { PWM_488_HZ,
       PWM_3906_HZ,
       PWM_31250_HZ };

/*** @brief configure pins and pwm for motor drive
 *  - set the motor driver pins as outputs
 *  - configure direction to be forwards
 *  - set pwm frequency to default 32kHz
 *  - set pwm drive to zero
 */
void setup_motors();

/*** @brief set the battery compensation scale factor
 *
 *  requires: battery_volts in Volts.
 *  returns: calculated scale factor
 * When requesting specific a voltage for a motor, the functions
 * set_left_motor_volts and set_right_motor_volts will calculate
 * a pwm value that tries to take into account the current
 * battery voltage. This is done by multiplying the desired
 * motor voltage by a scale factor.
 *
 * Before setting motor voltages, this function should be
 * called with the current battery voltage to set the scale
 * factor correctly.
 *
 * Without this, motor drive is likely to be too high when the
 * batteries are fresh and too low when the charge becomes depleted.
 */
float set_motor_battery_comp(float battery_volts);

/*** @brief write zero volts to both motors
 */
void stop_motors();

/*** @brief set the motor pwn drive to one of three possible values
 */
void set_motor_pwm_frequency(int frequency = PWM_31250_HZ);

/*** @brief set motor direction and PWM
 * -255 <= pwm <= 255
 */
void set_left_motor_pwm(int pwm);
void set_right_motor_pwm(int pwm);

/*** @brief adjust the motor PWM to deliver the given volate to the motor
 * The input voltage be any value and will be scaled to compensate for changes
 * in battery voltage such that maximum drive to the motors is limited to the
 * value of MAX_MOTOR_VOLTS in the defaults (normally +/- 6.0 Volts)
 */
void set_left_motor_volts(float volts);
void set_right_motor_volts(float volts);
#endif
