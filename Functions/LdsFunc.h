#pragma once

#include "../LdsTypes.h"

// List of function arguments
typedef CDList<string> CLdsInlineArgs;

// Global script function
struct LDS_API SLdsFunc {
  int ef_iArgs; // amount of arguments
  LdsReturn (*ef_pFunc)(CLdsValue *pArgs); // pointer to the function

  // Constructors
  SLdsFunc(void) : ef_iArgs(0), ef_pFunc(NULL) {};
  SLdsFunc(int ct, void *pFunc) : ef_iArgs(ct), ef_pFunc((LdsReturn (*)(CLdsValue *))pFunc) {};
};

// Inline function
struct LDS_API SLdsInlineFunc {
  CLdsInlineArgs in_astrArgs; // list of arguments
  CActionList in_acaFunc; // function actions

  CLdsInFuncMap in_mapInlineFunc; // Inline functions within this function

  // Constructors
  SLdsInlineFunc(void) {};
  SLdsInlineFunc(CLdsInlineArgs &astrArgs, CActionList &aca) {
    in_astrArgs.CopyArray(astrArgs);
    in_acaFunc.CopyArray(aca);
  };
};
