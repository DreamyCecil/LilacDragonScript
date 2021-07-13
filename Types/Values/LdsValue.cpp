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

// Constructor
CLdsValue::CLdsValue(void) :
  val_pBase(new CLdsIntType(0)) {};

// Simple constructors
CLdsValue::CLdsValue(const int &i) :
  val_pBase(new CLdsIntType(i)) {};

CLdsValue::CLdsValue(const float &f) :
  val_pBase(new CLdsFloatType(f)) {};

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
CLdsValue &CLdsValue::Assert(const ELdsValueType &eDesired) {
  if (val_pBase->GetType() == eDesired) {
    return *this;
  }
  
  // type mismatch
  string strExpected = ::TypeName(eDesired, "a number", "a string", "an array", "a structure");
  string strGot = val_pBase->TypeName("a number", "a string", "an array", "a structure");
  
  LdsThrow(LER_TYPE, "Expected %s but got %s", strExpected.c_str(), strGot.c_str());

  return *this;
};

CLdsValue &CLdsValue::AssertNumber(void) {
  if (val_pBase->GetType() <= EVT_FLOAT) {
    return *this;
  }

  // type mismatch
  string strGot = val_pBase->TypeName("a number", "a string", "an array", "a structure");
  LdsThrow(LER_TYPE, "Expected a number but got %s", strGot.c_str());

  return *this;
};

// Get integer value
int CLdsValue::GetIndex(void) const {
  return val_pBase->GetIndex();
};

// Get float value
float CLdsValue::GetNumber(void) const {
  return val_pBase->GetNumber();
};

// Get string value
string &CLdsValue::GetStringClass(void) const {
  return ToString().strValue;
};

// Get pure string value
const char *CLdsValue::GetString(void) const {
  return val_pBase->GetString();
};

// Get array value
CLdsArray &CLdsValue::GetArray(void) const {
  return ToArray().aArray;
};

// Get struct value
CLdsStruct &CLdsValue::GetStruct(void) const {
  return ToStruct().sStruct;
};

// Assignment
CLdsValue &CLdsValue::operator=(const CLdsValue &valOther) {
  if (this == &valOther) {
    return *this;
  }

  ILdsValueBase *pvalBase = valOther.val_pBase;
  
  // replace the value
  switch (pvalBase->GetType()) {
    case EVT_INDEX:  operator=(((CLdsIntType    *)pvalBase)->iValue); break;
    case EVT_FLOAT:  operator=(((CLdsFloatType  *)pvalBase)->fValue); break;
    case EVT_STRING: operator=(((CLdsStringType *)pvalBase)->strValue); break;
    case EVT_ARRAY:  operator=(((CLdsArrayType  *)pvalBase)->aArray); break;
    case EVT_STRUCT: operator=(((CLdsStructType *)pvalBase)->sStruct); break;
  }

  return *this;
};

// Assignment by value
CLdsValue &CLdsValue::operator=(const int &i) {
  DeleteValue();
  val_pBase = new CLdsIntType(i);

  return *this;
};

CLdsValue &CLdsValue::operator=(const float &f) {
  DeleteValue();
  val_pBase = new CLdsFloatType(f);

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
