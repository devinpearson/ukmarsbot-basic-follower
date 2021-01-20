
#ifndef Twiddle_h
#define Twiddle_h
#include "Arduino.h"
typedef float(EvalFunc)();

//********************************************************************** /
/***
 *
 * Twiddle will attempt to optimise some function by varying the parameters
 * used by that function.
 *
 * Before creating an instance of Twiddle, decide on up to 8 variables that
 * may be adjusted by the class.
 *
 * Create an array of pointers to those variables. Note that Twiddle will
 * change their values in-place but keep a copy of the original values so
 * that they can be restored so long as the instance of Twiddle is still in
 * scope.
 *
 * Write an evaluation function that takes no arguments and which returns a
 * floating point value that is the 'goodness' of the parameters. This function
 * will be called by the Twiddle instance many times during the optimisation.
 *
 *
 */
class Twiddle {
 public:
  Twiddle(int paramCount, float *iparams[], EvalFunc *evaluator);
  void logStep();
  void logTrial(float err);
  float *params[8];
  float deltas[8];
  float mBestError;
  int mParamCount = 8;
  float go();
  float sumDeltas();
  float maxDeltas();
  float evaluate();
  int mIteration;
  float mTwiddleFactor = 1.1;
  EvalFunc *mEval = nullptr;
};

#endif