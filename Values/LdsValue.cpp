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

// Value type name
string TypeName(const ELdsValueType &eType,
                const string &strNumber, const string &strString,
                const string &strArray, const string &strStruct)
{
  switch (eType) {
    case EVT_INDEX:
    case EVT_FLOAT: return strNumber;
    case EVT_STRING: return strString;
    case EVT_ARRAY: return strArray;
    case EVT_STRUCT: return strStruct;
  }
  return "<undefined>";
};

// Binary operation error
void LdsBinaryError(const CLdsValue &val1, const CLdsValue &val2, const CLdsToken &tkn) {
  LdsThrow(LEX_BINARY, "Cannot perform a binary operation %d on %s and %s at %s",
           tkn->GetIndex(), val1->TypeName().c_str(), val2->TypeName().c_str(), tkn.PrintPos().c_str());
};

// Constructor
CLdsValue::CLdsValue(void) :
  val_pBase(new CLdsIntType(0)) {};

// Value constructor
CLdsValue::CLdsValue(const ILdsValueBase &val) :
  val_pBase(val.MakeCopy()) {};

// Simple constructors
CLdsValue::CLdsValue(const int &i) :
  val_pBase(new CLdsIntType(i)) {};

CLdsValue::CLdsValue(const double &d) :
  val_pBase(new CLdsFloatType(d)) {};

CLdsValue::CLdsValue(const string &str) :
  val_pBase(new CLdsStringType(str)) {};

// Array constructor
CLdsValue::CLdsValue(const int &ct, CLdsValue valDef) :
  val_pBase(new CLdsArrayType(ct, valDef)) {};
  
// Structure constructor
CLdsValue::CLdsValue(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic) :
  val_pBase(new CLdsStructType(iSetID, map, bSetStatic)) {};

// Copy constructor
CLdsValue::CLdsValue(const CLdsValue &valOther) : val_pBase(NULL) {
  operator=(valOther);
};

// Destructor
CLdsValue::~CLdsValue(void) {
  DeleteValue();
};

// Delete the value
void CLdsValue::DeleteValue(void) {
  if (val_pBase != NULL) {
    delete val_pBase;
    val_pBase = NULL;
  }
};

// Type assertion (for function arguments)
CLdsValue &CLdsValue::Assert(const ILdsValueBase &valDesired) {
  if (val_pBase->GetType() == valDesired.GetType()) {
    return *this;
  }
  
  // type mismatch
  string strExpected = valDesired.TypeName();
  string strGot = val_pBase->TypeName();
  
  LdsThrow(LER_TYPE, "Expected %s but got %s", strExpected.c_str(), strGot.c_str());

  return *this;
};

// Assignment
CLdsValue &CLdsValue::operator=(const CLdsValue &valOther) {
  if (this == &valOther) {
    return *this;
  }

  // replace the value
  DeleteValue();
  val_pBase = valOther->MakeCopy();

  return *this;
};

// Assignment by value
CLdsValue &CLdsValue::operator=(const int &i) {
  DeleteValue();
  val_pBase = new CLdsIntType(i);

  return *this;
};

CLdsValue &CLdsValue::operator=(const double &d) {
  DeleteValue();
  val_pBase = new CLdsFloatType(d);

  return *this;
};

CLdsValue &CLdsValue::operator=(const string &str) {
  DeleteValue();
  val_pBase = new CLdsStringType(str);

  return *this;
};

CLdsValue &CLdsValue::operator=(const CLdsArray &a) {
  DeleteValue();
  val_pBase = new CLdsArrayType(a);

  return *this;
};

CLdsValue &CLdsValue::operator=(const CLdsStruct &s) {
  DeleteValue();
  val_pBase = new CLdsStructType(s);

  return *this;
};
  
bool CLdsValue::operator==(const CLdsValue &valOther) const {
  if (val_pBase->GetType() != valOther.val_pBase->GetType()) {
    return false;
  }
  
  return val_pBase->Compare(*valOther.val_pBase);
};
  
bool CLdsValue::operator!=(const CLdsValue &valOther) const {
  return !(operator==(valOther));
};
