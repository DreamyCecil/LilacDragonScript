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
#include "LdsObject.h"

// Dummy object callback function
static void DummyObjectCallback(CLdsObject *poObject, const int &iVariable) {
  (void)poObject;
};

// Constructors
CLdsObject::CLdsObject(void) :
  iID(-1), pCallback(&DummyObjectCallback) {};

CLdsObject::CLdsObject(const int &iSetID) :
  iID(iSetID), pCallback(&DummyObjectCallback) {};

// Clear the object
void CLdsObject::Clear(void) {
  iID = -1;
  aFields.Clear();
  pCallback = &DummyObjectCallback;
};

// Print one property
string CLdsObject::Print(int iVar) {
  SLdsVar &var = aFields[iVar];

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
CLdsObject &CLdsObject::operator=(const CLdsObject &sOther) {
  if (this == &sOther) {
    return *this;
  }

  // clear current fields
  Clear();
  
  iID = sOther.iID;
  aFields = sOther.aFields;

  return *this;
};

// Property accessor
CLdsValue &CLdsObject::operator[](const string &strVar) {
  return aFields.Find(strVar)->var_valValue;
};

// Find property index
int CLdsObject::FindIndex(const string &strVar) {
  return aFields.FindIndex(strVar);
};
