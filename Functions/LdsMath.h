#pragma once

#include "../Types/LdsValue.h"
#include "../Types/LdsValueRef.h"

// Standard math functions for expressions
#include <math.h>

// Trigonometric functions
inline LdsReturn LdsSin(LDS_ARGS) {
  return sinf(LDS_NEXT_NUM);
};

inline LdsReturn LdsCos(LDS_ARGS) {
  return cosf(LDS_NEXT_NUM);
};

inline LdsReturn LdsTan(LDS_ARGS) {
  return tanf(LDS_NEXT_NUM);
};

inline LdsReturn LdsASin(LDS_ARGS) {
  return asinf(LDS_NEXT_NUM);
};

inline LdsReturn LdsACos(LDS_ARGS) {
  return acosf(LDS_NEXT_NUM);
};

inline LdsReturn LdsATan(LDS_ARGS) {
  return atanf(LDS_NEXT_NUM);
};

inline LdsReturn LdsATan2(LDS_ARGS) {
  float fArg1 = LDS_NEXT_NUM;
  float fArg2 = LDS_NEXT_NUM;
  return atan2f(fArg1, fArg2);
};

// Power functions
inline LdsReturn LdsSqrt(LDS_ARGS) {
  return sqrtf(LDS_NEXT_NUM);
};

inline LdsReturn LdsRoot(LDS_ARGS) {
  float fNum = LDS_NEXT_NUM;
  float fBase = LDS_NEXT_NUM;

  // get number sign and turn into positive if needed
  float fSign = (fNum < 0.0f ? -1.0f : 1.0f);
  fNum = fabs(fNum);

  // example: [8, 3] = 2; [-8, 3] = -2
  return powf(fNum, 1.0f / fBase) * fSign;
};

inline LdsReturn LdsPow(LDS_ARGS) {
  float fArg1 = LDS_NEXT_NUM;
  float fArg2 = LDS_NEXT_NUM;
  return powf(fArg1, fArg2);
};

// Exponential functions
inline LdsReturn LdsExp(LDS_ARGS) {
  return expf(LDS_NEXT_NUM);
};

inline LdsReturn LdsLog(LDS_ARGS) {
  return logf(LDS_NEXT_NUM);
};

inline LdsReturn LdsLog2(LDS_ARGS) {
  // result of log2(e)
  #define LOG2_OF_E 1.44269504088896340736

  return float(log(LDS_NEXT_NUM) * LOG2_OF_E);
};

inline LdsReturn LdsLog10(LDS_ARGS) {
  return log10f(LDS_NEXT_NUM);
};

// Rounding functions
inline LdsReturn LdsCeil(LDS_ARGS) {
  return ceilf(LDS_NEXT_NUM);
};

inline LdsReturn LdsRound(LDS_ARGS) {
  return floorf(LDS_NEXT_NUM + 0.5f);
};

inline LdsReturn LdsFloor(LDS_ARGS) {
  return floorf(LDS_NEXT_NUM);
};

// Other
inline LdsReturn LdsAbs(LDS_ARGS) {
  return (float)fabs(LDS_NEXT_NUM);
};

inline LdsReturn LdsMin(LDS_ARGS) {
  float fArg1 = LDS_NEXT_NUM;
  float fArg2 = LDS_NEXT_NUM;
  return (fArg1 > fArg2) ? fArg2 : fArg1;
};

inline LdsReturn LdsMax(LDS_ARGS) {
  float fArg1 = LDS_NEXT_NUM;
  float fArg2 = LDS_NEXT_NUM;
  return (fArg1 > fArg2) ? fArg1 : fArg2;
};

inline LdsReturn LdsClamp(LDS_ARGS) {
  float fNum = LDS_NEXT_NUM;
  float fDown = LDS_NEXT_NUM;
  float fUp = LDS_NEXT_NUM;
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
