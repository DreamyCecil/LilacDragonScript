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

#include "StdH.h"
#include "LdsVar.h"

// Copy constructor
CLdsVars::CLdsVars(const CLdsVars &aOther) {
  operator=(aOther);
};

// Assignment
CLdsVars &CLdsVars::operator=(const CLdsVars &aOther) {
  if (this == &aOther) {
    return *this;
  }

  aVars = aOther.aVars;

  return *this;
};

// Add variables from another list
void CLdsVars::AddFrom(CLdsVars &aOther, const bool &bReplace) {
  int ctAdd = aOther.Count();

  // for each variable
  for (int iAdd = 0; iAdd < ctAdd; iAdd++) {
    SLdsVar &var = aOther.aVars[iAdd];
    string strVar = var.var_strName;

    // if should be replaced
    if (bReplace) {
      int iVar = FindIndex(strVar);

      // change the value
      if (iVar != -1) {
        aVars[iVar] = var;
        continue;
      }
    }

    // just add a new variable
    Add(var);
  }
};

// Delete variable by name
void CLdsVars::Delete(const string &strVar) {
  int iVar = FindIndex(strVar);

  if (iVar != -1) {
    aVars.Delete(iVar);
  }
};

// Get variable by name
SLdsVar *CLdsVars::Find(const string &strVar) {
  // go through variables
  for (int iVar = 0; iVar < aVars.Count(); iVar++) {
    SLdsVar &var = aVars[iVar];

    // return matching variable
    if (var.var_strName == strVar) {
      return &var;
    }
  }

  return NULL;
};

// Get variable index by name
int CLdsVars::FindIndex(const string &strVar) {
  // go through variables
  for (int iVar = 0; iVar < aVars.Count(); iVar++) {
    SLdsVar &var = aVars[iVar];

    // return matching variable
    if (var.var_strName == strVar) {
      return iVar;
    }
  }

  return -1;
};

// Default constructor
SLdsVar::SLdsVar(void) : var_strName(""), var_valValue(0), var_bConst(0) {};
  
// Value constructor
SLdsVar::SLdsVar(const string &strName, CLdsValue val, const bool &bConst) :
  var_strName(strName), var_valValue(val), var_bConst(bConst) {};
  
// Mark constants as set
void SLdsVar::SetConst(void) {
  if (var_bConst != 0) {
    var_bConst = 2;
  }
};
  
// Assignment
SLdsVar &SLdsVar::operator=(const SLdsVar &varOther) {
  if (this == &varOther) {
    return *this;
  }
    
  var_strName = varOther.var_strName;
  var_valValue = varOther.var_valValue;
  var_bConst = varOther.var_bConst;
    
  return *this;
};
