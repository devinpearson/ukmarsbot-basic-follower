#include "encoders.h"
#include <arduino.h>

#include "settings.h"

/****************************************************************************/
/*   ENCODERS                                                               */
/****************************************************************************/

/*
   from ATMega328p datasheet section 12:
   The ATMega328p can generate interrupt as a result of changes of state on two of its pins:

   PD2 for INT0  - Arduino Digital Pin 2
   PD3 for INT1  - Arduino Digital Pin 3

   The INT0 and INT1 interrupts can be triggered by a falling or rising edge or a low level.
   This is set up as indicated in the specification for the External Interrupt Control Register A –
   EICRA.

   The External Interrupt 0 is activated by the external pin INT0 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT0 pin that activate
   the interrupt are defined as

   ISC01 ISC00  Description
     0     0    Low Level of INT0 generates interrupt
     0     1    Logical change of INT0 generates interrupt
     1     0    Falling Edge of INT0 generates interrupt
     1     1    Rising Edge of INT0 generates interrupt


   The External Interrupt 1 is activated by the external pin INT1 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT1 pin that activate
   the interrupt are defined in Table 12-1

   ISC11 ISC10  Description
     0     0    Low Level of INT1 generates interrupt
     0     1    Logical change of INT1 generates interrupt
     1     0    Falling Edge of INT1 generates interrupt
     1     1    Rising Edge of INT1 generates interrupt

   To enable these interrupts, bits must be set in the external interrupt mask register EIMSK

   EIMSK:INT0 (bit 0) enables the INT0 external interrupt
   EIMSK:INT1 (bit 1) enables the INT1 external interrupt

*/

float encoderLeftSpeed;
float encoderRightSpeed;
float encoderPosition;
float encoderAngle;
int encoderLeftCount;
int encoderRightCount;
float encoderSpeed;
float encoderOmega;
int32_t encoderTotal;
int32_t encoderRotation;

/***
 * NOTE: the number of samples used for averaging has a big effect on
 * the PD controller tuning. Only change this with caution.
 * More samples introduces delay so gain must be reduced.
 */
const int AVERAGER_SAMPLES = 8;
int encoderIndex;
int encoderSpeedSum;
int encoderOmegaSum;
int8_t encoderSumHistory[AVERAGER_SAMPLES];
int8_t encoderDiffHistory[AVERAGER_SAMPLES];

void encoderReset() {
  noInterrupts();
  encoderLeftCount = 0;
  encoderRightCount = 0;
  encoderPosition = 0;
  encoderAngle = 0;
  for (int i = 0; i < AVERAGER_SAMPLES; i++) {
    encoderSumHistory[i] = 0;
    encoderDiffHistory[i] = 0;
  }
  encoderSpeedSum = 0;
  encoderOmegaSum = 0;
  interrupts();
}

void encoderSetup() {
  // left
  pinMode(ENCODER_LEFT_CLK, INPUT);
  pinMode(ENCODER_LEFT_B, INPUT);
  bitClear(EICRA, ISC01);
  bitSet(EICRA, ISC00);
  bitSet(EIMSK, INT0);
  // right
  pinMode(ENCODER_RIGHT_CLK, INPUT);
  pinMode(ENCODER_RIGHT_B, INPUT);
  bitClear(EICRA, ISC11);
  bitSet(EICRA, ISC10);
  bitSet(EIMSK, INT1);
  encoderReset();
}

// units are all in counts and counts per second
void encoderUpdate() {
  noInterrupts();
  encoderLeftSpeed = encoderLeftCount;
  encoderRightSpeed = encoderRightCount;
  encoderLeftCount = 0;
  encoderRightCount = 0;
  interrupts();
  int8_t encoderSum = encoderRightSpeed + encoderLeftSpeed;
  int8_t encoderDiff = encoderRightSpeed - encoderLeftSpeed;
  encoderTotal += encoderSum;
  encoderRotation += encoderDiff;
  // calculate the moving average speeds
  // Forward motion
  encoderSpeedSum -= encoderSumHistory[encoderIndex];
  encoderSumHistory[encoderIndex] = encoderSum;
  encoderSpeedSum += encoderSum;
  encoderSpeed = encoderSpeedSum * (1.0f / AVERAGER_SAMPLES);

  // encoderSpeed = encoderSum;
  encoderSpeed *= settings.mmPerCount;
  encoderPosition += encoderSpeed;
  encoderSpeed *= LOOP_FREQUENCY;

  // Angular motion

  encoderOmegaSum -= encoderDiffHistory[encoderIndex];
  encoderDiffHistory[encoderIndex] = encoderDiff;
  encoderOmegaSum += encoderDiffHistory[encoderIndex];
  encoderOmega = encoderOmegaSum;
  encoderOmega *= 1.0f / AVERAGER_SAMPLES;
  encoderOmega *= settings.degPerCount;
  encoderAngle += encoderOmega;
  encoderOmega *= LOOP_FREQUENCY;

  encoderIndex += 1;
  if (encoderIndex >= AVERAGER_SAMPLES) {
    encoderIndex = 0;
  }
}

float getEncoderSpeed() {
  return encoderSpeed;
}

float getEncoderOmega() {
  return encoderOmega;
}

// INT0 will respond to the XOR-ed pulse train from the leftencoder
// runs in constant time of around 3us per interrupt.
// would be faster with direct port access
ISR(INT0_vect) {
  static bool oldA = 0;
  static bool oldB = 0;
  bool newB = digitalRead(ENCODER_LEFT_B);
  bool newA = digitalRead(ENCODER_LEFT_CLK) ^ newB;
  int delta = ENCODER_LEFT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
  encoderLeftCount += delta;
  oldA = newA;
  oldB = newB;
}

// INT1 will respond to the XOR-ed pulse train from the right encoder
// runs in constant time of around 3us per interrupt.
// would be faster with direct port access
ISR(INT1_vect) {
  static bool oldA = 0;
  static bool oldB = 0;
  bool newB = digitalRead(ENCODER_RIGHT_B);
  bool newA = digitalRead(ENCODER_RIGHT_CLK) ^ newB;
  int delta = ENCODER_RIGHT_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
  encoderRightCount += delta;
  oldA = newA;
  oldB = newB;
}
