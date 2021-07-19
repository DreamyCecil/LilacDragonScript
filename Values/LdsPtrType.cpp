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

// Print the value
string CLdsPtrType::Print(void) {
  // TODO: Print index from the variable list or the variable name
  return LdsPrintF("%p", pValue);
};

// Perform a unary operation
CLdsValueRef CLdsPtrType::UnaryOp(CLdsValueRef &valRef, CCompAction &ca) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsPtrType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  // prioritize strings
  if (val2->GetType() == EVT_STRING) {
    return val2->BinaryOp(valRef1, valRef2, tkn);
  }

  // get value pointers
  ILdsValueBase *pVal1 = val1->GetPointer();
  ILdsValueBase *pVal2 = val2->GetPointer();

  switch (iOperation) {
    // conditional operators
    case LOP_EQ:  val1 = (pVal1 == pVal2); break;
    case LOP_NEQ: val1 = (pVal1 != pVal2); break;
      
    default: LdsBinaryError(val1, val2, tkn);
  }

  return CLdsValueRef(val1);
};
