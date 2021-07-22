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
#include "LdsStruct.h"

// Dummy structure callback function
static void DummyStructCallback(CLdsStruct *sStruct, const int &iVariable) {
  (void)0;
};

// Constructors
CLdsStruct::CLdsStruct(void) :
  iID(-1), pCallback(&DummyStructCallback) {};

CLdsStruct::CLdsStruct(const int &iSetID) :
  iID(iSetID), pCallback(&DummyStructCallback) {};

// Clear the structure
void CLdsStruct::Clear(void) {
  iID = -1;
  aFields.Clear();
  pCallback = &DummyStructCallback;
};

// Print one variable
string CLdsStruct::Print(int iVar) {
  SLdsVar &var = aFields.aVars[iVar];

  // print value
  CLdsValue &val = var.var_valValue;
  string strValue = val->Print();

  // surround with quotes
  if (val->GetType() == EVT_STRING) {
    strValue = '"' + strValue + '"';
  }
  
  // var = "val"
  return var.var_strName + " = " + strValue;
};

// Assignment
CLdsStruct &CLdsStruct::operator=(const CLdsStruct &sOther) {
  if (this == &sOther) {
    return *this;
  }

  // copy variable fields
  Clear();
  
  iID = sOther.iID;
  aFields = sOther.aFields;

  return *this;
};

// Structure accessor
CLdsValue &CLdsStruct::operator[](const string &strVar) {
  return aFields.Find(strVar)->var_valValue;
};

// Find variable index
int CLdsStruct::FindIndex(const string &strVar) {
  return aFields.FindIndex(strVar);
};
