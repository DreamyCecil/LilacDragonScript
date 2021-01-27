#pragma once

#include "../LdsTypes.h"

// Inline function call
struct SLdsInlineCall {
  string strFunc; // inline function name
  CDList<string> astrLocals; // inline function locals 
  CDStack<CLdsValueRef> avalStack; // stack of inline values
  
  CActionList acaReturn; // actions to return
  int iPos; // position to return to
  
  // Constructors
  SLdsInlineCall(void) : strFunc(""), iPos(0) {};
  SLdsInlineCall(const string &strName, const int &iSetPos) : strFunc(strName), iPos(iSetPos) {};
  
  // Destructor
  ~SLdsInlineCall(void) {
    acaReturn.Clear();
    astrLocals.Clear();
    avalStack.Clear();
  };
  
  // Inline variable name
  string VarName(string strVar) {
    return (strFunc + ":" + strVar);
  };
};
