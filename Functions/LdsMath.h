/* Copyright (c) 2021 Dreamy Cecil

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#pragma once

#include "../Values/LdsValue.h"
#include "../Types/LdsValueRef.h"

// Use STL log2 function or not
#if defined(_MSC_VER) && _MSC_VER >= 1700
  #define USE_LOG2_FUNC 1
#else
  #define USE_LOG2_FUNC 0
#endif

// Trigonometric functions
inline LdsReturn LdsSin(LDS_ARGS) {
  return sin(LDS_NEXT_NUM);
};

inline LdsReturn LdsCos(LDS_ARGS) {
  return cos(LDS_NEXT_NUM);
};

inline LdsReturn LdsTan(LDS_ARGS) {
  return tan(LDS_NEXT_NUM);
};

inline LdsReturn LdsASin(LDS_ARGS) {
  return asin(LDS_NEXT_NUM);
};

inline LdsReturn LdsACos(LDS_ARGS) {
  return acos(LDS_NEXT_NUM);
};

inline LdsReturn LdsATan(LDS_ARGS) {
  return atan(LDS_NEXT_NUM);
};

inline LdsReturn LdsATan2(LDS_ARGS) {
  double dArg1 = LDS_NEXT_NUM;
  double dArg2 = LDS_NEXT_NUM;
  return atan2(dArg1, dArg2);
};

// Power functions
inline LdsReturn LdsSqrt(LDS_ARGS) {
  return sqrt(LDS_NEXT_NUM);
};

inline LdsReturn LdsRoot(LDS_ARGS) {
  double dNum = LDS_NEXT_NUM;
  double dBase = LDS_NEXT_NUM;

  // get number sign and turn into positive if needed
  double dSign = (dNum < 0.0 ? -1.0 : 1.0);
  dNum = fabs(dNum);

  // example: (8, 3) = 2; (-8, 3) = -2
  return pow(dNum, 1.0 / dBase) * dSign;
};

inline LdsReturn LdsPow(LDS_ARGS) {
  double dArg1 = LDS_NEXT_NUM;
  double dArg2 = LDS_NEXT_NUM;
  return pow(dArg1, dArg2);
};

// Exponential functions
inline LdsReturn LdsExp(LDS_ARGS) {
  return exp(LDS_NEXT_NUM);
};

inline LdsReturn LdsLog(LDS_ARGS) {
  return log(LDS_NEXT_NUM);
};

inline LdsReturn LdsLog2(LDS_ARGS) {
#if USE_LOG2_FUNC == 1
  return log2(LDS_NEXT_NUM);

#else
  // result of log2(e)
  #define LOG2_OF_E 1.44269504088896340736

  // not using "log2()" for compatibility with older compilers
  return log(LDS_NEXT_NUM) * LOG2_OF_E;
#endif
};

inline LdsReturn LdsLog10(LDS_ARGS) {
  return log10(LDS_NEXT_NUM);
};

// Rounding functions
inline LdsReturn LdsCeil(LDS_ARGS) {
  return ceil(LDS_NEXT_NUM);
};

inline LdsReturn LdsRound(LDS_ARGS) {
  return floor(LDS_NEXT_NUM + 0.5f);
};

inline LdsReturn LdsFloor(LDS_ARGS) {
  return floor(LDS_NEXT_NUM);
};

// Other
inline LdsReturn LdsAbs(LDS_ARGS) {
  return fabs(LDS_NEXT_NUM);
};

inline LdsReturn LdsMin(LDS_ARGS) {
  double dArg1 = LDS_NEXT_NUM;
  double dArg2 = LDS_NEXT_NUM;
  return (dArg1 > dArg2) ? dArg2 : dArg1;
};

inline LdsReturn LdsMax(LDS_ARGS) {
  double dArg1 = LDS_NEXT_NUM;
  double dArg2 = LDS_NEXT_NUM;
  return (dArg1 > dArg2) ? dArg1 : dArg2;
};

inline LdsReturn LdsClamp(LDS_ARGS) {
  double dNum = LDS_NEXT_NUM;
  double dDown = LDS_NEXT_NUM;
  double dUp = LDS_NEXT_NUM;
  return (dNum >= dDown ? (dNum <= dUp ? dNum : dUp) : dDown);
};

// Math functions
inline void SetMathFunctions(CLdsFuncMap &map) {
  map.Add("atan2") = SLdsFunc(2, &LdsATan2);
  
  map.Add("root") = SLdsFunc(2, &LdsRoot);
  map.Add("pow") = SLdsFunc(2, &LdsPow);
  
  map.Add("min") = SLdsFunc(2, &LdsMin);
  map.Add("max") = SLdsFunc(2, &LdsMax);
  map.Add("clamp") = SLdsFunc(3, &LdsClamp);
};

// Math operators
inline void SetMathOperators(CLdsFuncPtrMap &map) {
  map.Add("sin") = &LdsSin;
  map.Add("cos") = &LdsCos;
  map.Add("tan") = &LdsTan;
  map.Add("asin") = &LdsASin;
  map.Add("acos") = &LdsACos;
  map.Add("atan") = &LdsATan;
  
  map.Add("sqrt") = &LdsSqrt;
  
  map.Add("exp") = &LdsExp;
  map.Add("log") = &LdsLog;
  map.Add("log2") = &LdsLog2;
  map.Add("log10") = &LdsLog10;
  
  map.Add("ceil") = &LdsCeil;
  map.Add("round") = &LdsRound;
  map.Add("floor") = &LdsFloor;
  
  map.Add("abs") = &LdsAbs;
};
