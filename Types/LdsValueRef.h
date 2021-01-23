#pragma once

#include "LdsValue.h"

// Constructor templates
#define ARGS_TEMP(_Var, _Access, _Name, _Ref, _Const) pvar(_Var), pvalAccess(_Access), strVar(_Name), strRef(_Ref), bConst(_Const)
#define CONSTRUCTOR_TEMP(_Type) SLdsValueRef(const _Type &_Set) : \
                                             val(_Set), ARGS_TEMP(NULL, NULL, "", "", false) {}
#define CONSTRUCTOR_FULL(_Type) SLdsValueRef(const _Type &_Set, SLdsVar *_Var, CLdsValue *_Access, const string &_Name, const string &_Ref, bool _Const) : \
                                             val(_Set), ARGS_TEMP(_Var, _Access, _Name, _Ref, _Const) {}

// Value reference
struct LDS_API SLdsValueRef {
  CLdsValue val; // value itself
  
  SLdsVar *pvar; // variable reference
  CLdsValue *pvalAccess; // array/structure value reference

  string strVar; // variable this value belongs to
  string strRef; // reference name

  bool bConst; // reference is a constant variable (for structure variables)
  
  // Default constructor
  SLdsValueRef(void) : val(0.0f), ARGS_TEMP(NULL, NULL, "", "", false) {};
  
  // Value constructors
  CONSTRUCTOR_TEMP(CLdsValue);
  CONSTRUCTOR_TEMP(int);
  CONSTRUCTOR_TEMP(float);
  CONSTRUCTOR_TEMP(string);
  
  // Full constructors
  CONSTRUCTOR_FULL(CLdsValue);
  CONSTRUCTOR_FULL(int);
  CONSTRUCTOR_FULL(float);
  CONSTRUCTOR_FULL(string);
};

// Undefine constructor templates
#undef CONSTRUCTOR_TEMP
#undef CONSTRUCTOR_FULL
#undef ARGS_TEMP

// A function return type to distinguish the purpose
typedef SLdsValueRef LdsReturn;
