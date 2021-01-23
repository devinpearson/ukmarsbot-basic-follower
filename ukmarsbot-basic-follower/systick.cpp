#include "systick.h"
#include <arduino.h>
#include "encoders.h"
#include "motion.h"
#include "profile.h"
#include "sensors.h"

volatile uint32_t ticks = 0;
void setupSystick(int frequency) {
  bitClear(TCCR2A, WGM20);
  bitSet(TCCR2A, WGM21);
  bitClear(TCCR2B, WGM22);
  // set divisor to 128 => 125kHz
  bitSet(TCCR2B, CS22);
  bitClear(TCCR2B, CS21);
  bitSet(TCCR2B, CS20);
  OCR2A = 249;  // (16000000/128/500)-1 => 500Hz
  // OCR2A = 124;    // (16000000/128/1000)-1 => 1000 Hz
  bitSet(TIMSK2, OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
  ticks++;
  sensorsUpdate();
  encoderUpdate();
  steeringUpdate();
  rot.adjust(gSteeringControl);
  fwd.update();
  rot.update();
  motionUpdate();
}