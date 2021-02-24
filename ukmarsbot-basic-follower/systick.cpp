#include "systick.h"
#include "encoders.h"

#include "motors.h"
#include "profile.h"
#include "sensors.h"
#include <arduino.h>

void setup_systick() {
  bitClear(TCCR2A, WGM20);
  bitSet(TCCR2A, WGM21);
  bitClear(TCCR2B, WGM22);
  // set divisor to 128 => 125kHz
  bitSet(TCCR2B, CS22);
  bitClear(TCCR2B, CS21);
  bitSet(TCCR2B, CS20);
  OCR2A = 249; // (16000000/128/500)-1 => 500Hz
  bitSet(TIMSK2, OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
  sensorsUpdate();
  set_motor_battery_comp(batteryVolts);
  encoderUpdate();
  steeringUpdate();
  rot.adjust(gSteeringControl);
  fwd.update();
  rot.update();
  update_motor_controllers();
}