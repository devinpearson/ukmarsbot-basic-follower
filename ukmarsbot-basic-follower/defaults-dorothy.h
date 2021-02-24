#ifndef DOROTHY
#define DOROTHY

#define SETTINGS_VERSION 0x01

#define DEFAULTS_ROBOT_MODE MODE_MAZE
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

#define WHEEL_DIAMETER 32.0
#define ENCODER_PULSES 12.0
#define GEAR_RATIO 20.0
#define MOUSE_RADIUS 37.0

// forward motion uses the sum of the two encoders for odometry hence the 2.0 constant
#define DEFAULTS_MM_PER_COUNT (PI * WHEEL_DIAMETER / (2.0 * ENCODER_PULSES * GEAR_RATIO))

// rotation uses the difference between the encoders
#define DEFAULTS_DEG_PER_COUNT ((360.0 * DEFAULTS_MM_PER_COUNT) / (2.0 * PI * MOUSE_RADIUS))

#define DEFAULTS_FWD_KP 1.0
#define DEFAULTS_FWD_KD 5.0

#define DEFAULTS_ROT_KP 0.7
#define DEFAULTS_ROT_KD 2.0

// controller constants for the line follower configuration
#define DEFAULTS_LINE_KP 1.5
#define DEFAULTS_LINE_KD 6.0
#define DEFAULTS_LINE_ALPHA 0.1 // sensor error filter constant

// controller constants for the line follower configuration
#define DEFAULTS_WALL_KP 0.6
#define DEFAULTS_WALL_KD 6.0
#define DEFAULTS_WALL_CTE_GAIN 0.0005

// time delay for sensors to respond to emitters
#define DEFAULTS_EMITTER_ON_TIME 50

// Motor Feedforward
#define DEFAULTS_LEFT_FF_SPEED_FWD 0.00347
#define DEFAULTS_LEFT_FF_SPEED_REV 0.00347
#define DEFAULTS_LEFT_FF_ACC_FWD 0.000362
#define DEFAULTS_LEFT_FF_ACC_REV 0.000362
#define DEFAULTS_LEFT_FF_STATIC_FWD 0.317
#define DEFAULTS_LEFT_FF_STATIC_REV 0.317

#define DEFAULTS_RIGHT_FF_SPEED_FWD 0.00347
#define DEFAULTS_RIGHT_FF_SPEED_REV 0.00347
#define DEFAULTS_RIGHT_FF_ACC_FWD 0.000362
#define DEFAULTS_RIGHT_FF_ACC_REV 0.000362
#define DEFAULTS_RIGHT_FF_STATIC_FWD 0.317
#define DEFAULTS_RIGHT_FF_STATIC_REV 0.317

// Line sensor thresholds
const float DEFAULTS_LINE_WIDTH = 19.0;              // ADJUST THIS so that CTE is roughly equal to the error in mm
const float DEFAULTS_LINE_DETECT_THRESHOLD = 900.0;  // minimum value to register the line - ADJUST TO SUIT
const float DEFAULTS_LEFT_MARKER_THRESHOLD = 180.0;  // minimum value to register the turn marker
const float DEFAULTS_RIGHT_MARKER_THRESHOLD = 180.0; // minimum value to register the start marker

// wall sensor thresholds and constants
// the default values for the front sensor when the robot is backed up to a wall
const int FRONT_REFERENCE = 100.0;
// the default values for the side sensors when the robot is centred in a cell
// and there is no wall ahead
const float LEFT_REFERENCE = 82.0;
const float RIGHT_REFERENCE = 101.0;

// Sensor brightness adjustment factor. The compiler calculates these so it saves processor time
const float FRONT_ADJUST = 100.0 / FRONT_REFERENCE;
const float LEFT_ADJUST = 100.0 / LEFT_REFERENCE;
const float RIGHT_ADJUST = 100.0 / RIGHT_REFERENCE;
// This is the value seen by the front sensor when the mouse is centred, facing a wall
const float FRONT_NOMINAL = 250.0;
const float LEFT_NOMINAL = 100.0;
const float RIGHT_NOMINAL = 100.0;

// the values above which, a wall is seen
const int FRONT_WALL_THRESHOLD = 20; // minimum value to register a wall
const int LEFT_WALL_THRESHOLD = 40;  // minimum value to register a wall
const int RIGHT_WALL_THRESHOLD = 40; // minimum value to register a wall

// search and run speeds
const float TURN_SPEED = 300;
const float SEARCH_SPEED = 400;
const float SEARCH_ACCEL = 2000;
const float ACCEL_DIST = (SEARCH_SPEED * SEARCH_SPEED) / (2 * SEARCH_ACCEL);                           // 63mm
const float BRAKE_DIST = (SEARCH_SPEED * SEARCH_SPEED - TURN_SPEED * TURN_SPEED) / (2 * SEARCH_ACCEL); // 63mm

#endif