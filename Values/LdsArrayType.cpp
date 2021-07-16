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
string CLdsArrayType::Print(void) {
  int ctArray = aArray.Count();
        
  if (ctArray <= 0) {
    return "[ ]";
  }

  // array opening
  string strArray = "[ ";
        
  for (int iArray = 0; iArray < ctArray; iArray++) {
    if (iArray != 0) {
      // next entry
      strArray += ", ";
    }
          
    // print array entry
    strArray += aArray[iArray].Print();
  }
        
  // array closing
  strArray += " ]";

  return strArray;
};

// Perform a unary operation
CLdsValueRef CLdsArrayType::UnaryOp(CLdsValueRef &valRef, CCompAction &ca) {
  // [Cecil] TEMP: Cannot do unary operations on arrays
  LdsThrow(LEX_UNARY, "Cannot perform a unary operation on an array at %s", ca.PrintPos().c_str());

  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  int iOperation = ca->GetIndex();

  switch (iOperation) {
    // TODO: Make array inversion
    case UOP_INVERT: break;
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsArrayType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, CCompAction &ca) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = ca->GetIndex();

  // types
  const string strType1 = val1->TypeName("a number", "a string", "an array", "a structure");
  const string strType2 = val2->TypeName("a number", "a string", "an array", "a structure");

  CLdsValue *pvalArrayAccess = NULL;
    
  switch (iOperation) {
    // TODO: Add multiplication operator to copy array contents multiple times
    // operators
    case LOP_ADD: {
      if (val2->GetType() > EVT_FLOAT) {
        LdsThrow(LEX_BINARY, "Cannot add %s to an array at %s", strType2.c_str(), ca.PrintPos().c_str());
      }
        
      // expand the array
      int ctResize = (val1->GetArray().Count() + val2->GetIndex());
      val1->GetArray().Resize(ctResize);
    } break;
        
    case LOP_SUB: {
      if (val2->GetType() > EVT_FLOAT) {
        LdsThrow(LEX_BINARY, "Cannot subtract %s from an array at %s", strType2.c_str(), ca.PrintPos().c_str());
      }
        
      // shrink the array
      int ctResize = (val1->GetArray().Count() - val2->GetIndex());
      val1->GetArray().Resize(ctResize);
    } break;
      
    // conditional operators
    case LOP_GT: case LOP_GOE:
    case LOP_LT: case LOP_LOE:
    case LOP_EQ: case LOP_NEQ:
      if (val2->GetType() != EVT_ARRAY) {
        LdsThrow(LEX_BINARY, "Cannot compare an array with %s at %s", strType2.c_str(), ca.PrintPos().c_str());
      }
        
      switch (iOperation) {
        case LOP_GT:  val1 = int(val1->GetArray().Count() >  val2->GetArray().Count()); break;
        case LOP_GOE: val1 = int(val1->GetArray().Count() >= val2->GetArray().Count()); break;
        case LOP_LT:  val1 = int(val1->GetArray().Count() <  val2->GetArray().Count()); break;
        case LOP_LOE: val1 = int(val1->GetArray().Count() <= val2->GetArray().Count()); break;
        case LOP_EQ:  val1 = int(val1 == val2); break;
        case LOP_NEQ: val1 = int(val1 != val2); break;
      }
      break;
      
    // accessor
    case LOP_ACCESS: {
      if (ca.lt_iArg >= 1) {
        LdsThrow(LEX_BINARY, "Cannot use structure accessor on the array at %s", ca.PrintPos().c_str());
      }
        
      if (val2->GetType() > EVT_FLOAT) {
        LdsThrow(LEX_BINARY, "Cannot use %s as an array accessor at %s", strType2.c_str(), ca.PrintPos().c_str());
      }
        
      // direct 'val1 = val1.aArray' empties its own array before getting a value from it
      CLdsArray aCopy = val1->GetArray();
        
      int iArrayIndex = val2->GetIndex();
      int iSize = aCopy.Count();
    
      // out of bounds
      if (iSize <= 0) {
        LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty array at %s", ca.PrintPos().c_str());

      } else if (iArrayIndex < 0 || iArrayIndex >= iSize) {
        LdsThrow(LEX_ARRAYOUT, "Array index '%d' is out of bounds [0, %d] at %s", iArrayIndex, iSize-1, ca.PrintPos().c_str());
      }
        
      val1 = aCopy[iArrayIndex];
        
      // get pointer to the value within the array
      pvalArrayAccess = valRef1.GetValue(iArrayIndex);

      // add reference index
      valRef1.AddIndex(iArrayIndex);
    } break;
      
    default: LdsThrow(LEX_BINARY, "Cannot perform a binary operation %d on %s and %s at %s", ca->GetIndex(), strType1.c_str(), strType2.c_str(), ca.PrintPos().c_str());
  }

  return CLdsValueRef(val1, valRef1.vr_pvar, pvalArrayAccess, valRef1.vr_strVar, valRef1.vr_strRef, valRef1.IsConst(), valRef1.IsGlobal());
};
