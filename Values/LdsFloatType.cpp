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
string CLdsFloatType::Print(void) {
  return LdsPrintF("%s", LdsFloatStr(dValue).c_str());
};

// Perform a unary operation
CLdsValueRef CLdsFloatType::UnaryOp(CLdsValueRef &valRef, CCompAction &ca) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  int iOperation = ca->GetIndex();

  switch (iOperation) {
    case UOP_NEGATE:
      val = -val->GetNumber();
      break;

    case UOP_INVERT: {
      bool bInvert = (val->GetNumber() > 0.5);
      val = (int)!bInvert;
    } break;
    
    // invert bits of the double
    case UOP_BINVERT: {
      double dInvert = val->GetNumber();

      LONG64 iInvert = ~(reinterpret_cast<LONG64 &>(dInvert));
      dInvert = reinterpret_cast<double &>(iInvert);

      val = dInvert;
    } break;
    
    // turn char index into a char string
    case UOP_STRINGIFY: {
      int iChar = val->GetIndex();

      char strChar[2];
      SPRINTF_FUNC(strChar, "%c", iChar);

      val = string(strChar);
    } break;
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsFloatType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // use integer's binary function
  return CLdsIntType(0).BinaryOp(valRef1, valRef2, tkn);
};
