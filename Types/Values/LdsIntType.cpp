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
string CLdsIntType::Print(void) {
  return LdsPrintF("%d", iValue);
};

// Perform a binary operation
CLdsValueRef CLdsIntType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, CCompAction &ca) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  // use secondary value's binary function if it's not a number
  if (val2->GetType() > EVT_FLOAT) {
    return val2->BinaryOp(valRef1, valRef2, ca);
  }

  int iOperation = ca->GetIndex();

  // get numbers
  int iNum1 = val1->GetIndex();
  int iNum2 = val2->GetIndex();
  double dNum1 = val1->GetNumber();
  double dNum2 = val2->GetNumber();

  switch (iOperation) {
    // operators
    case LOP_ADD: val1 = (dNum1 + dNum2); break;
    case LOP_SUB: val1 = (dNum1 - dNum2); break;
    case LOP_MUL: val1 = (dNum1 * dNum2); break;
    case LOP_DIV: val1 = (dNum1 / dNum2); break;
    
    case LOP_FMOD: {
      if (dNum2 != 0.0f) {
        val1 = fmod(dNum1, dNum2);
      } else {
        val1 = 0.0f;
      }
    } break;
      
    case LOP_IDIV: {
      if (iNum2 != 0) {
        val1 = (iNum1 / iNum2);
      } else {
        val1 = 0;
      }
    } break;
        
    // bitwise operators
    case LOP_SH_L:  val1 = (iNum1 << iNum2); break;
    case LOP_SH_R:  val1 = (iNum1 >> iNum2); break;
    case LOP_B_AND: val1 = (iNum1 &  iNum2); break;
    case LOP_B_XOR: val1 = (iNum1 ^  iNum2); break;
    case LOP_B_OR:  val1 = (iNum1 |  iNum2); break;

    // conditional operators
    case LOP_AND: val1 = (iNum1 > 0) && (iNum2 > 0); break;
    case LOP_OR:  val1 = (iNum1 > 0) || (iNum2 > 0); break;
    case LOP_XOR: val1 = ((iNum1 >= 0) ^ (iNum2 > 0)) > 0; break;

    case LOP_GT:  val1 = (dNum1 >  dNum2); break;
    case LOP_GOE: val1 = (dNum1 >= dNum2); break;
    case LOP_LT:  val1 = (dNum1 <  dNum2); break;
    case LOP_LOE: val1 = (dNum1 <= dNum2); break;
    case LOP_EQ:  val1 = (dNum1 == dNum2); break;
    case LOP_NEQ: val1 = (dNum1 != dNum2); break;
      
    default: LdsThrow(LEX_BINARY, "Cannot apply operator %d at %s", iOperation, ca.PrintPos().c_str());
  }

  return CLdsValueRef(val1);
};
