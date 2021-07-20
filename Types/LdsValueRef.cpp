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

// Constructor
SLdsRefIndex::SLdsRefIndex(void) :
  strIndex("0"), bIndex(true) {};

// Structure variable constructor
SLdsRefIndex::SLdsRefIndex(const string &strVar) :
  strIndex(strVar), bIndex(false) {};

// Array index constructor
SLdsRefIndex::SLdsRefIndex(const int &iIndex) : bIndex(true)
{
  // write index into a stream
  std::ostringstream strStream;
  strStream << iIndex;

  // write index as a string
  strIndex = strStream.str();
};

// Get value by an array index
CLdsValue *CLdsValueRef::AccessValue(const int &iIndex) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      CLdsArray &aArray = (*vr_pvalAccess)->GetArray();
      return &aArray[iIndex];

    } else {
      return &vr_pvar->var_valValue->GetArray()[iIndex];
    }
  }

  return &vr_val->GetArray()[iIndex];
};
    
// Get value by a structure variable name
CLdsValue *CLdsValueRef::AccessValue(const string &strVar) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      CLdsStruct &sStruct = (*vr_pvalAccess)->GetStruct();
      return &sStruct[strVar];

    } else {
      return &vr_pvar->var_valValue->GetStruct()[strVar];
    }
  }
  
  return &vr_val->GetStruct()[strVar];
};
