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
