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

// Variable list
class LDS_API CLdsVars {
  public:
    DSList<SLdsVar> aVars;
    
  public:
    // Default constructor
    CLdsVars(void) {};

    // Copy constructor
    CLdsVars(const CLdsVars &aOther);

    // Assignment
    CLdsVars &operator=(const CLdsVars &aOther);

    // Add a new variable
    inline int Add(const SLdsVar &varNew);

    // Delete variable by index
    inline void Delete(const int &iPos) {
      aVars.Delete(iPos);
    };

    // Clear variables
    inline void Clear(void) {
      aVars.Clear();
    };

    // Count variables
    inline int Count(void) const {
      return aVars.Count();
    };

    // Get index of some variable
    inline int Index(SLdsVar *pvar) const {
      return aVars.Index(pvar);
    };

    // Get variable by index
    SLdsVar &operator[](const int &iPos) {
      return aVars[iPos];
    };

    // Add variables from another list
    void AddFrom(CLdsVars &aOther, const bool &bReplace = true);

    // Delete variable by name
    void Delete(const string &strVar);

    // Get variable by name
    SLdsVar *Find(const string &strVar);

    // Get variable index by name
    int FindIndex(const string &strVar);
};

// Script variable
struct LDS_API SLdsVar {
  string var_strName; // variable name
  CLdsValue var_valValue; // value

  char var_bConst; // is value constant (0 - no, 1 - yes, not set, 2 - yes, set)

  // Default constructor
  SLdsVar(void);
  
  // Value constructor
  SLdsVar(const string &strName, CLdsValue val, const bool &bConst = false);
  
  // Mark constants as set
  void SetConst(void);
  
  // Assignment
  SLdsVar &operator=(const SLdsVar &varOther);
};

// Inline definitions

// Add a new variable
inline int CLdsVars::Add(const SLdsVar &varNew) {
  return aVars.Add(varNew);
};
