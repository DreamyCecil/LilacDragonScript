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
#include "LdsValueRef.h"

// Default constructor
SLdsRefIndex::SLdsRefIndex(void) :
  strIndex("0"), bIndex(true) {};

// Property constructor
SLdsRefIndex::SLdsRefIndex(const string &strVar) :
  strIndex(strVar), bIndex(false) {};

// Index constructor
SLdsRefIndex::SLdsRefIndex(const int &iIndex) : bIndex(true)
{
  // write index into a stream
  std::ostringstream strStream;
  strStream << iIndex;

  // write index as a string
  strIndex = strStream.str();
};


// Constructors
CLdsValueRef::CLdsValueRef(void) :
  vr_val(CLdsIntType()), vr_pvar(NULL), vr_pvarAccess(NULL), vr_ubFlags(0) {};

CLdsValueRef::CLdsValueRef(const CLdsValue &val) :
  vr_val(val), vr_pvar(NULL), vr_pvarAccess(NULL), vr_ubFlags(0) {};

CLdsValueRef::CLdsValueRef(const CLdsValue &val, SLdsVar *pvar, SLdsVar *pvarAccess, const LdsFlags &ubFlags) :
  vr_val(val), vr_pvar(pvar), vr_pvarAccess(pvarAccess), vr_ubFlags(ubFlags) {};

// Get variable by index
SLdsVar *CLdsValueRef::AccessVariable(const int &iIndex) {
  CLdsVars *paVars = &vr_val->GetVars();

  if (vr_pvar != NULL) {
    if (vr_pvarAccess != NULL) {
      paVars = &vr_pvarAccess->var_valValue->GetVars();

    } else {
      paVars = &vr_pvar->var_valValue->GetVars();
    }
  }
  
  return &(*paVars)[iIndex];
};
    
// Get variable by name
SLdsVar *CLdsValueRef::AccessVariable(const string &strVar) {
  CLdsVars *paVars = &vr_val->GetVars();

  if (vr_pvar != NULL) {
    if (vr_pvarAccess != NULL) {
      paVars = &vr_pvarAccess->var_valValue->GetVars();

    } else {
      paVars = &vr_pvar->var_valValue->GetVars();
    }
  }
  
  return paVars->Find(strVar);
};
