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
#include "LdsIntType.h"

// Write value into the stream
void CLdsIntType::Write(LdsEnginePtr pEngine, void *pStream) {
  pEngine->_pLdsWrite(pStream, &iValue, sizeof(int));
};

// Read value from the stream
void CLdsIntType::Read(LdsEnginePtr pEngine, void *pStream, CLdsValue &val) {
  int i = 0;
  pEngine->_pLdsRead(pStream, &i, sizeof(int));

  val = i;
};

// Print the value
string CLdsIntType::Print(void) {
  return LdsPrintF("%d", iValue);
};

// Perform a unary operation
CLdsValueRef CLdsIntType::UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  string strOperator = tkn->GetString();

  IF_UN("-") {
    val = -val->GetIndex();

  } ELSE_UN("!") {
    val = (int)!val->IsTrue();

  } ELSE_UN("~") {
    int iInvert = val->GetIndex();
    val = ~iInvert;

  // turn char index into a char string
  } ELSE_UN("$") {
    int iChar = val->GetIndex();

    char strChar[2];
    SPRINTF_FUNC(strChar, "%c", iChar);

    val = string(strChar);

  } else {
    LdsUnaryError(val, tkn);
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsIntType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  // prioritize strings
  if (val2->GetType() == EVT_STRING) {
    return val2->BinaryOp(valRef1, valRef2, tkn);
  }

  // get integers
  int iNum1 = val1->GetIndex();
  int iNum2 = val2->GetIndex();

  switch (iOperation) {
    case LOP_ADD: case LOP_SUB: case LOP_MUL: case LOP_DIV: case LOP_FMOD:
    case LOP_GT: case LOP_GOE: case LOP_LT: case LOP_LOE: case LOP_EQ: case LOP_NEQ: {
      // prioritize float numbers
      if (val2->GetType() == EVT_FLOAT) {
        return val2->BinaryOp(valRef1, valRef2, tkn);
      }

      // integers
      switch (iOperation) {
        // operators
        case LOP_ADD: val1 = (iNum1 + iNum2); break;
        case LOP_SUB: val1 = (iNum1 - iNum2); break;
        case LOP_MUL: val1 = (iNum1 * iNum2); break;
        case LOP_DIV: val1 = (iNum1 / iNum2); break;
    
        case LOP_FMOD: {
          if (iNum2 != 0) {
            val1 = (iNum1 % iNum2);
          } else {
            val1 = 0;
          }
        } break;
      
        // conditional operators
        case LOP_AND: val1 = (iNum1 > 0 && iNum2 > 0); break;
        case LOP_OR:  val1 = (iNum1 > 0 || iNum2 > 0); break;
        case LOP_XOR: val1 = XOR_CHECK(iNum1 >= 0, iNum2 > 0); break;

        case LOP_GT:  val1 = (iNum1 >  iNum2); break;
        case LOP_GOE: val1 = (iNum1 >= iNum2); break;
        case LOP_LT:  val1 = (iNum1 <  iNum2); break;
        case LOP_LOE: val1 = (iNum1 <= iNum2); break;
        case LOP_EQ:  val1 = (iNum1 == iNum2); break;
        case LOP_NEQ: val1 = (iNum1 != iNum2); break;
      }
    } break;

    // integer division
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
      
    default: LdsBinaryError(val1, val2, tkn);
  }

  return CLdsValueRef(val1);
};
