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
CLdsValue *CLdsValueRef::GetValue(const int &iIndex) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      CLdsArray &aArray = (*vr_pvalAccess)->GetArray();
      return &aArray[iIndex];

    } else {
      return &vr_pvar->var_valValue->GetArray()[iIndex];
    }
  }

  // TODO: Make sure this is a wise decision instead of just using NULL
  // TODO: Check if something like 'val = ExternalFunc()[0][1];' worked before when it was NULL
  return &vr_val->GetArray()[iIndex];
};
    
// Get value by a structure variable name
CLdsValue *CLdsValueRef::GetValue(const string &strVar) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      CLdsStruct &sStruct = (*vr_pvalAccess)->GetStruct();
      return &sStruct[strVar];

    } else {
      return &vr_pvar->var_valValue->GetStruct()[strVar];
    }
  }
  
  // TODO: Make sure this is a wise decision instead of just using NULL
  // TODO: Check if something like 'val = ExternalFunc().structVar[1];' worked before when it was NULL
  return &vr_val->GetStruct()[strVar];
};

// Convert into the variable pointer
CLdsValue CLdsValueRef::ToPointer(void) {
  // pointer to the structure variable
  if (vr_strRef != "") {
    // get last reference
    SLdsRefIndex &ri = vr_ariIndices[vr_ariIndices.Count() - 1];

    CLdsValue &val = vr_pvar->var_valValue;

    // array index
    if (ri.bIndex) {
      LdsThrow(LEX_POINTER, "Cannot retrive pointer to an array value at %s", LdsPrintPos(LDS_iActionPos).c_str());

    // structure field
    } else {
      SLdsVar &varStruct = val->GetStruct().mapVars[vr_strRef];
      return CLdsPtrType(&varStruct);
    }
  }

  // pointer to the variable
  return CLdsPtrType(vr_pvar);
};
