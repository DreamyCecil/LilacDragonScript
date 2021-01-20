#pragma once

#include "../Types/LdsValue.h"
#include "../Types/LdsValueRef.h"

// Standard math functions for expressions
#include <math.h>

// Trigonometric functions
inline LdsReturn LdsSin(SLdsValue *pArgs) {
  return sinf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsCos(SLdsValue *pArgs) {
  return cosf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsTan(SLdsValue *pArgs) {
  return tanf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsASin(SLdsValue *pArgs) {
  return asinf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsACos(SLdsValue *pArgs) {
  return acosf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsATan(SLdsValue *pArgs) {
  return atanf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsATan2(SLdsValue *pArgs) {
  float fArg1 = LDS_NEXTNUM(pArgs);
  float fArg2 = LDS_NEXTNUM(pArgs);
  return atan2f(fArg1, fArg2);
};

// Power functions
inline LdsReturn LdsSqrt(SLdsValue *pArgs) {
  return sqrtf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsRoot(SLdsValue *pArgs) {
  float fNum = LDS_NEXTNUM(pArgs);
  float fBase = LDS_NEXTNUM(pArgs);

  // get number sign and turn into positive if needed
  float fSign = (fNum < 0.0f ? -1.0f : 1.0f);
  fNum = fabs(fNum);

  // example: [8, 3] = 2; [-8, 3] = -2
  return powf(fNum, 1.0f / fBase) * fSign;
};

inline LdsReturn LdsPow(SLdsValue *pArgs) {
  float fArg1 = LDS_NEXTNUM(pArgs);
  float fArg2 = LDS_NEXTNUM(pArgs);
  return powf(fArg1, fArg2);
};

// Exponential functions
inline LdsReturn LdsExp(SLdsValue *pArgs) {
  return expf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsLog(SLdsValue *pArgs) {
  return logf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsLog2(SLdsValue *pArgs) {
  // result of log2(e)
  #define LOG2_OF_E 1.44269504088896340736

  return float(log(LDS_NEXTNUM(pArgs)) * LOG2_OF_E);
};

inline LdsReturn LdsLog10(SLdsValue *pArgs) {
  return log10f(LDS_NEXTNUM(pArgs));
};

// Rounding functions
inline LdsReturn LdsCeil(SLdsValue *pArgs) {
  return ceilf(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsRound(SLdsValue *pArgs) {
  return floorf(LDS_NEXTNUM(pArgs) + 0.5f);
};

inline LdsReturn LdsFloor(SLdsValue *pArgs) {
  return floorf(LDS_NEXTNUM(pArgs));
};

// Other
inline LdsReturn LdsAbs(SLdsValue *pArgs) {
  return (float)fabs(LDS_NEXTNUM(pArgs));
};

inline LdsReturn LdsMin(SLdsValue *pArgs) {
  float fArg1 = LDS_NEXTNUM(pArgs);
  float fArg2 = LDS_NEXTNUM(pArgs);
  return (fArg1 > fArg2) ? fArg2 : fArg1;
};

inline LdsReturn LdsMax(SLdsValue *pArgs) {
  float fArg1 = LDS_NEXTNUM(pArgs);
  float fArg2 = LDS_NEXTNUM(pArgs);
  return (fArg1 > fArg2) ? fArg1 : fArg2;
};

inline LdsReturn LdsClamp(SLdsValue *pArgs) {
  float fNum = LDS_NEXTNUM(pArgs);
  float fDown = LDS_NEXTNUM(pArgs);
  float fUp = LDS_NEXTNUM(pArgs);
  return (fNum >= fDown ? (fNum <= fUp ? fNum : fUp) : fDown);
};

inline void SetMathFunctions(CLdsFuncMap &map) {
  map["Sin"] = SLdsFunc(1, &LdsSin);
  map["Cos"] = SLdsFunc(1, &LdsCos);
  map["Tan"] = SLdsFunc(1, &LdsTan);
  map["ASin"] = SLdsFunc(1, &LdsASin);
  map["ACos"] = SLdsFunc(1, &LdsACos);
  map["ATan"] = SLdsFunc(1, &LdsATan);
  map["ATan2"] = SLdsFunc(2, &LdsATan2);
  
  map["Sqrt"] = SLdsFunc(1, &LdsSqrt);
  map["Root"] = SLdsFunc(2, &LdsRoot);
  map["Pow"] = SLdsFunc(2, &LdsPow);
  
  map["Exp"] = SLdsFunc(1, &LdsExp);
  map["Log"] = SLdsFunc(1, &LdsLog);
  map["Log2"] = SLdsFunc(1, &LdsLog2);
  map["Log10"] = SLdsFunc(1, &LdsLog10);
  
  map["Ceil"] = SLdsFunc(1, &LdsCeil);
  map["Round"] = SLdsFunc(1, &LdsRound);
  map["Floor"] = SLdsFunc(1, &LdsFloor);
  
  map["Abs"] = SLdsFunc(1, &LdsAbs);
  map["Min"] = SLdsFunc(2, &LdsMin);
  map["Max"] = SLdsFunc(2, &LdsMax);
  map["Clamp"] = SLdsFunc(3, &LdsClamp);
};
