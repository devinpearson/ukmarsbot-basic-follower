#ifndef PROFILE_H
#define PROFILE_H

#include <stdint.h>

/*** @brief Provides a way to manage and encaapsulate speed profiles
 *
 * Simply setting the speed of a robot is not enough for full control
 * of a robot. Most significantly, the robot cannot instantaneously change
 * speed so the profiler manages the acceleration and deceleration of
 * the robot so that speed changes are achieved smoothly and with
 * regard for the physical capabilities of the robot.
 *
 * By placing the profiler into a C++ class, the code can be shared by
 * both forward and rotary motion while still keeping the state variables
 * separate.
 *
 * In practice the main task of the profiler is to make it possible to issue
 * commands like
 * "start moving with a target speed of x mm/s and acceleration of a mm/s/s"
 * "move s mm witha  maximum speed of x mm/s and a terminal speed of y mm/s"
 *
 * Because the same code is available for both forward and rotary motion, it is
 * easy to substitute degrees, degrees/second and degrees/s/s anywhere
 * that distances are described.
 *
 * Moves can be started and then left to continue or the profiler can be
 * made to wait until a move has finished.
 */
class Profile {
  public:
  enum { ACCELERATE,
         BRAKE,
         FINISHED };
  enum { MANUAL,
         AUTOMATIC };
  explicit Profile(float *feedback) : mFeedback(feedback) { reset(); }
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
  int8_t mMode = AUTOMATIC; // MANUAL or AUTOMATIC
  volatile int8_t mState = FINISHED;
  int8_t mDirection;
  float mKP;
  float mKD;
  float *mFeedback;
  float mError;
  float mOldError;

  float mLastInput;
  float mControlOutput;
};

extern Profile fwd;
extern Profile rot;

#endif