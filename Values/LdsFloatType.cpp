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

// Write value into the stream
void CLdsFloatType::Write(class CLdsScriptEngine *pEngine, void *pStream) {
  pEngine->_pLdsWrite(pStream, &dValue, sizeof(double));
};

// Read value from the stream
void CLdsFloatType::Read(class CLdsScriptEngine *pEngine, void *pStream, CLdsValue &val) {
  double d = 0.0;
  pEngine->_pLdsRead(pStream, &d, sizeof(double));

  val = d;
};

// Print the value
string CLdsFloatType::Print(void) {
  return LdsFloatStr(dValue);
};

// Perform a unary operation
CLdsValueRef CLdsFloatType::UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  string strOperator = tkn->GetString();

  IF_UN("-") {
    val = -val->GetNumber();

  } ELSE_UN("!") {
    bool bInvert = (val->GetNumber() > 0.5);
    val = (int)!bInvert;
    
  // invert bits of the double
  } ELSE_UN("~") {
    double dInvert = val->GetNumber();

    LONG64 llInvert = ~(reinterpret_cast<LONG64 &>(dInvert));
    dInvert = reinterpret_cast<double &>(llInvert);

    val = dInvert;
    
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
CLdsValueRef CLdsFloatType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();
  
  // prioritize strings
  if (val2->GetType() == EVT_STRING) {
    return val2->BinaryOp(valRef1, valRef2, tkn);
  }

  // get numbers
  double dNum1 = val1->GetNumber();
  double dNum2 = val2->GetNumber();
  int iNum1 = (int)dNum1;
  int iNum2 = (int)dNum2;

  switch (iOperation) {
    // operators
    case LOP_ADD: val1 = (dNum1 + dNum2); break;
    case LOP_SUB: val1 = (dNum1 - dNum2); break;
    case LOP_MUL: val1 = (dNum1 * dNum2); break;
    case LOP_DIV: val1 = (dNum1 / dNum2); break;
    
    case LOP_FMOD: {
      if (dNum2 != 0.0) {
        val1 = fmod(dNum1, dNum2);
      } else {
        val1 = 0.0;
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
    case LOP_AND: val1 = (dNum1 > 0.0 && dNum2 > 0.0); break;
    case LOP_OR:  val1 = (dNum1 > 0.0 || dNum2 > 0.0); break;
    case LOP_XOR: val1 = XOR_CHECK(dNum1 >= 0.0, dNum2 > 0.0); break;

    case LOP_GT:  val1 = (dNum1 >  dNum2); break;
    case LOP_GOE: val1 = (dNum1 >= dNum2); break;
    case LOP_LT:  val1 = (dNum1 <  dNum2); break;
    case LOP_LOE: val1 = (dNum1 <= dNum2); break;
    case LOP_EQ:  val1 = (dNum1 == dNum2); break;
    case LOP_NEQ: val1 = (dNum1 != dNum2); break;
      
    default: LdsBinaryError(val1, val2, tkn);
  }

  return CLdsValueRef(val1);
};
