#ifndef ARIADNE
#define ARIADNE

const int SENSOR_COUNT = 4;
const float MAX_MOTOR_VOLTS = 6.0;

const int EEPROM_ADDR_SETTINGS = 0x0000;
// encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
#define ENCODER_LEFT_POLARITY (-1)
#define ENCODER_RIGHT_POLARITY (1)

// similarly, the motors may be wired with different polarity and that is defined here so that
// setting a positive voltage always moves the robot forwards
#define MOTOR_LEFT_POLARITY (1)
#define MOTOR_RIGHT_POLARITY (-1)

#define SETTINGS_VERSION 0x03
#define WHEEL_DIAMETER 32.0
#define ENCODER_PULSES 12.0
#define GEAR_RATIO 20.0
#define MOUSE_RADIUS 37.0

// forward motion uses the sum of the two encoders for odometry hence the 2.0 constant
#define DEFAULTS_MM_PER_COUNT (PI * WHEEL_DIAMETER / (2.0 * ENCODER_PULSES * GEAR_RATIO))

// rotation uses the difference between the encoders
#define DEFAULTS_DEG_PER_COUNT ((360.0 * DEFAULTS_MM_PER_COUNT) / (2.0 * PI * MOUSE_RADIUS))

#define DEFAULTS_ROT_KP 0.35
#define DEFAULTS_ROT_KD 4.0

#define DEFAULTS_MOUSE_RADIUS (57.29583 * DEFAULTS_MM_PER_COUNT / DEFAULTS_DEG_PER_COUNT)
// controller constants for the line follower configuration
#define DEFAULTS_LINE_KP 1.5
#define DEFAULTS_LINE_KD 6.0
#define DEFAULTS_LINE_ALPHA 0.1 // sensor error filter constant

// controller constants for the line follower configuration
#define DEFAULTS_WALL_KP 1.5
#define DEFAULTS_WALL_KD 6.0

// time delay for sensors to respond to emitters
#define DEFAULTS_EMITTER_ON_TIME 50

// Motor Feedforward
#define DEFAULTS_SPEED_FF 0.00347


#endif