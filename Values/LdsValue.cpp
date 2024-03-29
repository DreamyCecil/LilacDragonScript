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

// Unary operation error
void LdsUnaryError(const CLdsValue &val, const CLdsToken &tkn) {
  LdsThrow(LEX_UNARY, "Cannot perform a unary operation '%s' on %s at %s",
           tkn->GetString().c_str(), val->TypeName().c_str(), tkn.PrintPos().c_str());
};

// Binary operation error
void LdsBinaryError(const CLdsValue &val1, const CLdsValue &val2, const CLdsToken &tkn) {
  LdsThrow(LEX_BINARY, "Cannot perform a binary operation '%s' on %s and %s at %s",
           _astrBinaryOps[tkn->GetIndex()], val1->TypeName().c_str(), val2->TypeName().c_str(), tkn.PrintPos().c_str());
};

// Get integer value
int ILdsValueBase::GetIndex(void) { return 0; };

// Get float value
double ILdsValueBase::GetNumber(void) { return 0.0; };

// Get string value
string ILdsValueBase::GetString(void) { return ""; };

// Get variables
CLdsVars *ILdsValueBase::GetVars(void) { return NULL; };

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

  // allow any numbers
  if (val_pBase->GetType() <= EVT_FLOAT && valDesired.GetType() <= EVT_FLOAT) {
    return *this;
  }
  
  // type mismatch
  const char *strExpected = valDesired.TypeName().c_str();
  const char *strGot = val_pBase->TypeName().c_str();
  
  LdsThrow(LER_VALUE, "Expected %s but got %s at %s", strExpected, strGot, LdsPrintPos(LDS_iActionPos).c_str());

  return *this;
};

// Variable list assertion (for function arguments)
CLdsVars &CLdsValue::AssertList(const int &ctMinVars) {
  CLdsVars *paVars = val_pBase->GetVars();

  const char *strGot = val_pBase->TypeName().c_str();
  const char *strPos = LdsPrintPos(LDS_iActionPos).c_str();

  // no value list
  if (paVars == NULL) {
    LdsThrow(LER_VALUE, "Expected a type with the value list but got %s at %s", strGot, strPos);
  }

  // not enough values
  if (ctMinVars > 0 && paVars->Count() < ctMinVars) {
    LdsThrow(LER_VALUE, "Expected at least %d values in %s but got %d at %s", ctMinVars, strGot, paVars->Count(), strPos);
  }

  return *val_pBase->GetVars();
};

// Assignment
CLdsValue &CLdsValue::operator=(const CLdsValue &valOther) {
  // replace the value
  DeleteValue();
  val_pBase = valOther->MakeCopy();

  return *this;
};

// Assignment by value
void CLdsValue::FromInt(const int &i) {
  DeleteValue();
  val_pBase = new CLdsIntType(i);
};

void CLdsValue::FromFloat(const double &d) {
  DeleteValue();
  val_pBase = new CLdsFloatType(d);
};

void CLdsValue::FromString(const string &str) {
  DeleteValue();
  val_pBase = new CLdsStringType(str);
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
