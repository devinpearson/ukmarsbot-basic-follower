
#include "motors.h"
#include "digitalWriteFast.h"
#include "encoders.h"
#include "pid.h"
#include "profile.h"
#include "settings.h"
#include <arduino.h>

PID fwd_controller(defaults.fwdKP, defaults.fwdKI, defaults.fwdKD, &encoderSpeed, &fwd.mCurrentSpeed);
PID rot_controller(defaults.rotKP, defaults.rotKI, defaults.rotKD, &encoderOmega, &rot.mCurrentSpeed);

bool motor_controllers_enabled;
bool feedforward_enabled;
volatile float left_motor_volts;
volatile float right_motor_volts;

void setup_motor_controllers() {
  motor_controllers_enabled = false;
  fwd_controller.set_tunings(settings.fwdKP, settings.fwdKI, settings.fwdKD);
  fwd_controller.set_tunings(settings.rotKP, settings.rotKI, settings.rotKD);
}

void update_motor_controllers() {
  // assume both motors behave the same
  const float k_velocity_ff = (1.0 / 280.0);

  fwd_controller.compute();
  rot_controller.compute();

  float fwd_volts = 0;
  float rot_volts = 0;

  fwd_volts += fwd_controller.output();
  rot_volts += rot_controller.output();

  float left_volts = fwd_volts - rot_volts;
  float right_volts = fwd_volts + rot_volts;

  float fwd_ff = fwd.mCurrentSpeed * k_velocity_ff;
  float rot_ff = rot.mCurrentSpeed * (MOUSE_RADIUS / (57.29)) * k_velocity_ff;

  left_volts += fwd_ff;
  right_volts += fwd_ff;

  left_volts -= rot_ff;
  right_volts += rot_ff;
  if (motor_controllers_enabled) {
    set_left_motor_volts(left_volts);
    set_right_motor_volts(right_volts);
  }
}

/****************************************************************************/
/*   MOTORS and PWM                                                         */
/****************************************************************************/

float motor_battery_comp = 255.0f / MAX_MOTOR_VOLTS;

void setup_motors() {
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  digitalWriteFast(MOTOR_LEFT_PWM, 0);
  digitalWriteFast(MOTOR_LEFT_DIR, 0);
  digitalWriteFast(MOTOR_RIGHT_PWM, 0);
  digitalWriteFast(MOTOR_RIGHT_DIR, 0);
  set_motor_pwm_frequency();
  stop_motors();
}

/**
 * Direct register access could be used here for enhanced performance
 */
void set_left_motor_pwm(int pwm) {
  pwm = MOTOR_LEFT_POLARITY * constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWriteFast(MOTOR_LEFT_DIR, 1);
    analogWrite(MOTOR_LEFT_PWM, -pwm);
  } else {
    digitalWriteFast(MOTOR_LEFT_DIR, 0);
    analogWrite(MOTOR_LEFT_PWM, pwm);
  }
}

void set_right_motor_pwm(int pwm) {
  pwm = MOTOR_RIGHT_POLARITY * constrain(pwm, -255, 255);
  if (pwm < 0) {
    digitalWriteFast(MOTOR_RIGHT_DIR, 1);
    analogWrite(MOTOR_RIGHT_PWM, -pwm);
  } else {
    digitalWriteFast(MOTOR_RIGHT_DIR, 0);
    analogWrite(MOTOR_RIGHT_PWM, pwm);
  }
}

float set_motor_battery_comp(float battery_volts) {
  motor_battery_comp = 255.0f / battery_volts;
  return motor_battery_comp;
}

void set_left_motor_volts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  left_motor_volts = volts;
  int motorPWM = motor_battery_comp * volts;
  set_left_motor_pwm(motorPWM);
}

void set_right_motor_volts(float volts) {
  volts = constrain(volts, -MAX_MOTOR_VOLTS, MAX_MOTOR_VOLTS);
  right_motor_volts = volts;
  int motorPWM = motor_battery_comp * volts;
  set_right_motor_pwm(motorPWM);
}

void set_motor_pwm_frequency(int frequency) {
  switch (frequency) {
    case PWM_31250_HZ:
      // Divide by 1. frequency = 31.25 kHz;
      bitClear(TCCR1B, CS11);
      bitSet(TCCR1B, CS10);
      break;
    case PWM_3906_HZ:
      // Divide by 8. frequency = 3.91 kHz;
      bitSet(TCCR1B, CS11);
      bitClear(TCCR1B, CS10);
      break;
    case PWM_488_HZ:
    default:
      // Divide by 64. frequency = 488Hz;
      bitSet(TCCR1B, CS11);
      bitSet(TCCR1B, CS10);
      break;
  }
}

void stop_motors() {
  set_left_motor_volts(0);
  set_right_motor_volts(0);
}

/****************************************************************************/
