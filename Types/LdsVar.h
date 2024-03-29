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

    // Add an empty variable
    inline SLdsVar &Add(void);

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
    inline int Index(SLdsVar &var) const {
      return aVars.FindIndex(var);
    };

    // Get variable by index
    SLdsVar &operator[](const int &iPos) {
      return aVars[iPos];
    };

    const SLdsVar &operator[](const int &iPos) const {
      return aVars[iPos];
    };

    // Add variables from another list
    void AddFrom(CLdsVars &aOther, const bool &bReplace = true);

    // Delete variable by name
    void Delete(const string &strVar);

    // Get variable by name
    SLdsVar *Find(const string &strVar);

    // Get variable index by name
    int FindIndex(const string &strVar) const;
};

// Script variable
struct LDS_API SLdsVar {
  string var_strName; // variable name
  CLdsValue var_valValue; // value

  char var_bConst; // is value constant (0 - no, 1 - yes, not set, 2 - yes, set)

  // Default constructor
  SLdsVar(void);
  
  // Property constructor
  SLdsVar(const string &strName, const CLdsValue &val, const bool &bConst = false);

  // Value constructor
  SLdsVar(const CLdsValue &val);
  
  // Mark constants as set
  void SetConst(void);
  
  // Assignment
  SLdsVar &operator=(const SLdsVar &varOther);

  // Assignment by value
  SLdsVar &operator=(const CLdsValue &valOther);

  // Quick value access
  inline ILdsValueBase *operator->(void) const {
    return var_valValue.val_pBase;
  };
  inline operator ILdsValueBase*(void) const {
    return var_valValue.val_pBase;
  };
  inline ILdsValueBase &operator*(void) const {
    return *var_valValue.val_pBase;
  };
};

// Inline definitions

// Add a new variable
inline int CLdsVars::Add(const SLdsVar &varNew) {
  return aVars.Add(varNew);
};

// Add an empty variable
inline SLdsVar &CLdsVars::Add(void) {
  return aVars.Add();
};
