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

#include <algorithm>

// Write value into the stream
void CLdsStringType::Write(class CLdsScriptEngine *pEngine, void *pStream) {
  pEngine->LdsWriteString(pStream, strValue);
};

// Read value from the stream
void CLdsStringType::Read(class CLdsScriptEngine *pEngine, void *pStream, CLdsValue &val) {
  string str = "";
  pEngine->LdsReadString(pStream, str);

  strValue = str;
};

// Print the value
string CLdsStringType::Print(void) {
  return strValue;
};

// Perform a unary operation
CLdsValueRef CLdsStringType::UnaryOp(CLdsValueRef &valRef, CCompAction &ca) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  int iOperation = ca->GetIndex();

  switch (iOperation) {
    // string inversion
    case UOP_INVERT: {
      string strString = val->GetString();
      std::reverse(strString.begin(), strString.end());

      val = strString;
    } break;

    // it's already a string
    case UOP_STRINGIFY: break;

    default: LdsThrow(LEX_UNARY, "Cannot perform a unary operation %d on a string at %s", ca->GetIndex(), ca.PrintPos().c_str());
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsStringType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  // types
  const string strType1 = val1->TypeName();
  const string strType2 = val2->TypeName();

  switch (iOperation) {
    // append to the string
    case LOP_ADD: {
      val1 = val1->Print() + val2->Print();
    } break;

    // remove characters from the end
    case LOP_SUB: {
      if (val2->GetType() == EVT_STRING) {
        LdsThrow(LEX_BINARY, "Cannot subtract a string from a string at %s", tkn.PrintPos().c_str());
      }

      string str = val1->GetString();
      int ctStr = str.length();
      int ctSub = val2->GetIndex();

      val1 = str.substr(0, ctStr - ctSub);
    } break;

    // copy the same string multiple times
    case LOP_MUL: {
      if (val2->GetType() == EVT_STRING) {
        LdsThrow(LEX_BINARY, "Cannot multiply a string by a string at %s", tkn.PrintPos().c_str());
      }

      string strMul = "";

      for (int iCopy = 0; iCopy < val2->GetIndex(); iCopy++) {
        strMul += val1->GetString();
      }

      val1 = strMul;
    } break;

    // compare strings
    case LOP_EQ: {
      bool bResult = false;

      switch (val2->GetType()) {
        // compare length
        case EVT_INDEX: case EVT_FLOAT:
          bResult = (val1->GetString().length() == val2->GetIndex());
          break;

        // compare string contents
        case EVT_STRING:
          bResult = (val1 == val2);
          break;

        default:
          LdsThrow(LEX_BINARY, "Cannot compare a string with %s at %s", strType2.c_str(), tkn.PrintPos().c_str());
      }

      val1 = bResult;
    } break;

    case LOP_NEQ: {
      bool bResult = false;

      switch (val2->GetType()) {
        // compare length
        case EVT_INDEX: case EVT_FLOAT:
          bResult = (val1->GetString().length() != val2->GetIndex());
          break;

        // compare string contents
        case EVT_STRING:
          bResult = (val1 != val2);
          break;

        default:
          LdsThrow(LEX_BINARY, "Cannot compare a string with %s at %s", strType2.c_str(), tkn.PrintPos().c_str());
      }

      val1 = bResult;
    } break;

    // accessor
    case LOP_ACCESS: {
      if (tkn.lt_iArg >= 1) {
        // TODO: Make it search for a certain substring and return amount of them in a string
        //       Example: "string".str - returns 1; "search for the sea"["sea"] - returns 2
        LdsThrow(LEX_BINARY, "Cannot use structure accessor on a string at %s", tkn.PrintPos().c_str());
      }

      if (val2->GetType() > EVT_FLOAT) {
        LdsThrow(LEX_BINARY, "Cannot use %s as an string accessor at %s", strType2.c_str(), tkn.PrintPos().c_str());
      }
        
      string strCopy = val1->GetString();
        
      int iCharIndex = val2->GetIndex();
      int iLength = strCopy.length();
    
      // out of bounds
      if (iLength <= 0) {
        LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty string at %s", tkn.PrintPos().c_str());

      } else if (iCharIndex < 0 || iCharIndex > iLength) {
        LdsThrow(LEX_ARRAYOUT, "Character index '%d' is out of bounds [0, %d] at %s", iCharIndex, iLength, tkn.PrintPos().c_str());
      }
        
      // get one character from the string (including terminating null character)
      val1 = (int)strCopy[iCharIndex];
    } break;

    default: LdsBinaryError(val1, val2, tkn);
  }

  return CLdsValueRef(val1);
};
