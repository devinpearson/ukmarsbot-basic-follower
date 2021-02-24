#include "twiddle.h"
#include "commands.h"

#include "settings.h"
/**********************************************************************/
Twiddle::Twiddle(int paramCount, float *iparams[], EvalFunc *evaluator = nullptr)  // Constructor
/**********************************************************************/
{
  mBestError = 9999.99;
  mIteration = 0;
  mTwiddleFactor = 1.1;
  mParamCount = paramCount;
  mEval = evaluator;
  for (int i = 0; i < mParamCount; i++) {
    params[i] = iparams[i];
    deltas[i] = *params[i] / 4.0f;
  }
}

//----------------------------------------------------------------
void Twiddle::logStep()
//----------------------------------------------------------------
{
  Serial.print(F(" Step = "));
  Serial.print(mIteration);
  Serial.print(F(" best Error "));
  Serial.print(mBestError, 4);
  for (int i = 0; i < mParamCount; i++) {
    Serial.print(' ');
    Serial.print(*params[i], 6);
  }
  Serial.print("  :  ");
  for (int i = 0; i < mParamCount; i++) {
    Serial.print(' ');
    Serial.print(deltas[i], 6);
  }
  Serial.println();
  Serial.println();
}

void Twiddle::logTrial(float err) {
  Serial.print(F("  Trial "));
  Serial.print(mIteration);
  Serial.print(F("      Error "));
  Serial.print(err, 4);
  for (int i = 0; i < mParamCount; i++) {
    Serial.print(' ');
    Serial.print(*params[i], 6);
  }
  Serial.print("  :  ");
  for (int i = 0; i < mParamCount; i++) {
    Serial.print(' ');
    Serial.print(deltas[i], 6);
  }
  Serial.println();
}

float Twiddle::sumDeltas() {
  float sum = 0;
  for (int i = 0; i < mParamCount; i++) {
    sum += deltas[i];
  }
  return sum;
}

float Twiddle::maxDeltas() {
  float max = 0;
  for (int i = 0; i < mParamCount; i++) {
    if (deltas[i] > max) {
      max = deltas[i] / *params[i];
    }
  }
  return max;
}

float Twiddle::go() {
  mIteration = 0;
  // get the starting conditions
  float err;
  mBestError = evaluate();
  logStep();
  while (maxDeltas() > 0.01) {
    mIteration++;
    // probe each parameter in turn
    for (int i = 0; i < mParamCount; i++) {
      // increase this parameter and see what happens
      *params[i] += deltas[i];
      err = evaluate();
      if (err <= mBestError) {
        mBestError = err;             // that made things better
        deltas[i] *= mTwiddleFactor;  // so try increasing it some more
      } else {
        *params[i] -= 2 * deltas[i];  // no better so go the other way
        if (*params[i] < 0) {
          *params[i] = 0;
        }
        err = evaluate();  // and see how that works out
        if (err <= mBestError) {
          mBestError = err;             // great - that worked
          deltas[i] *= mTwiddleFactor;  // so try a bigger step
        } else {
          *params[i] += deltas[i];      // both changes made things worse so
          deltas[i] /= mTwiddleFactor;  // restore the parameter and reduce the delta
        }
      }
    }
    // it will be clear that the algorithm may run several iterations
    // with few obvious improvements
    logStep();
  }
  //
  Serial.println(F("\n\nBest result:\n"));
  logStep();
  return mBestError;
}

float Twiddle::evaluate() {
  float err = 0;
  if (mEval != nullptr) {
    err = mEval();
  }
  logTrial(err);
  return err;
}
