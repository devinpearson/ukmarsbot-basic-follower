#ifndef MOTION_H
#define MOTION_H
#include "board.h"
#include "sensors.h"
/***
 * fwdSpeed is in mm/s
 * turnSpeed is in deg/s
 */

extern float leftFF;
extern float rightFF;

// Add constructor that tells profile where its feedback comes from.

class Profile {
 public:
  enum { ACCELERATE, BRAKE, FINISHED };
  enum { MANUAL, AUTOMATIC };
  explicit Profile(float* feedback) : mFeedback(feedback) { reset(); }
  void reset();
  /***
   * Begins a move sequence. The parameters are set up and the controllers initialised.
   * On exit the move will be underway with processing happening in the background.
   * Call the isFinished() method to find out when it is all over.
   */
  void startMove(float distance, float topSpeed, float endSpeed, float acceleration);

  /***
   * calls startMove() and then waits for the move to finish
   */
  void move(float distance, float topSpeed, float endSpeed, float acceleration);

  /***
   * Return true if the profile has finished processing. The robot may still be moving
   * but it will be at constant velocity.
   * A profile that has never started is also considered to be 'finished'
   */
  bool isFinished();

  /***
   * calculate the deceleration needed to bringthe profile to the specified end speed
   * given the current speed and available distance.
   */
  float getBraking(float distance, float speed, float endspeed);
  /***
   * Call update() in each system tick to process the profile.
   */
  void update();

  /***
   * adjust() is used to modify the profile. The adjustment term is a velocity which is
   * added to the profile velocity.
   * For the forward motion, this has no obvious purpose but for rotation, adjust() is
   * where the line or wall tracking adjustments happen.
   * Thus, a line followr would use the sensor error to adjust the angular velocity
   */
  void adjust(float adjustment);

  /***
   * controllerUpdate() is called every system tick from update()
   * It will calculate the output voltage needed for the motors for this profile.
   * Note that the forward and rotation profile output voltages must be combined
   * to get the individual motor drive voltages.
   */
  void controllerUpdate();

  /***
   * The profiler can be in one of two modes:
   * MANUAL    - the controller is not active and the user can set the output voltage
   * AUTOMATIC - the profile uses its PD controller to set the ouput voltage
   */
  void setMode(int mode);

  float mAcceleration;
  volatile float mCurrentSpeed;
  float mAdjustment;
  float mTargetSpeed;
  float mEndSpeed;
  volatile float mPosition;
  float mEndPosition;
  int8_t mMode = AUTOMATIC;  // MANUAL or AUTOMATIC
  volatile int8_t mState = FINISHED;
  int8_t mDirection;
  float mKP;
  float mKD;
  float* mFeedback;
  float mError;
  float mOldError;

  float mLastInput;
  float mControlOutput;
};

extern Profile fwd;
extern Profile rot;
extern bool motionEnabled;

void motionUpdate();
void motionInit();

#endif
