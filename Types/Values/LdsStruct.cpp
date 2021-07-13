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

// Clear the structure
void CLdsStruct::Clear(void) {
  iID = -1;
  mapVars.Clear();
};

// Print one variable
string CLdsStruct::Print(int iVar) {
  // print value
  string strValue = mapVars.GetValue(iVar).var_valValue.Print();
  
  // var = 'val'
  string strVar = mapVars.GetKey(iVar) + " = '" + strValue + "'";
  
  return strVar;
};

// Assignment
CLdsStruct &CLdsStruct::operator=(const CLdsStruct &sOther) {
  if (this == &sOther) {
    return *this;
  }

  // copy variable fields
  Clear();
  
  iID = sOther.iID;
  mapVars = sOther.mapVars;

  return *this;
};

// Structure accessor
CLdsValue &CLdsStruct::operator[](const string &strVar) {
  return mapVars[strVar].var_valValue;
};

// Find variable index
int CLdsStruct::FindIndex(const string &strVar) {
  return mapVars.FindKeyIndex(strVar);
};
