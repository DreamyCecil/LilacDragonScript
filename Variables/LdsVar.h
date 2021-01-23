#pragma once

#include "../Types/LdsValue.h"

// Script variable
struct LDS_API SLdsVar {
  CLdsValue var_valValue; // value
  char var_bConst; // is value constant (0 - no, 1 - yes, not set, 2 - yes, set)

  // Constructors
  SLdsVar(void) : var_valValue(0.0f), var_bConst(0) {};
  SLdsVar(CLdsValue val, bool bConst = false) : var_valValue(val), var_bConst(bConst) {};
  
  // Mark constants as set
  void SetConst(void) {
    if (var_bConst != 0) {
      var_bConst = 2;
    }
  };
  
  // Assignment
  SLdsVar &operator=(const SLdsVar &varOther) {
    if (this == &varOther) {
      return *this;
    }
    
    var_valValue = varOther.var_valValue;
    var_bConst = varOther.var_bConst;
    
    return *this;
  };
};
