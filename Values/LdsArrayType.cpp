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
#include "LdsArrayType.h"

// Write value into the stream
void CLdsArrayType::Write(LdsEnginePtr pEngine, void *pStream) {
  const int ctArray = aArray.Count();

  // write array count
  pEngine->_pLdsWrite(pStream, &ctArray, sizeof(int));

  // write each individual array value
  for (int i = 0; i < ctArray; i++) {
    pEngine->LdsWriteValue(pStream, aArray[i].var_valValue);
  }
};

// Read value from the stream
void CLdsArrayType::Read(LdsEnginePtr pEngine, void *pStream, CLdsValue &val) {
  // read array count
  int ctArray = 0;
  pEngine->_pLdsRead(pStream, &ctArray, sizeof(int));

  // create an empty array
  val = CLdsArrayType(ctArray, 0);

  // read values into the array
  for (int i = 0; i < ctArray; i++) {
    CLdsValue valArray;
    pEngine->LdsReadValue(pStream, valArray);

    val->GetVars()->Add() = valArray;
  }
};

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
    strArray += aArray[iArray].var_valValue->Print();
  }
        
  // array closing
  strArray += " ]";

  return strArray;
};

// Perform a unary operation
CLdsValueRef CLdsArrayType::UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  string strOperator = tkn->GetString();

  // reverse order of array values
  IF_UN("!") {
    CLdsVars aArrayCopy = *val->GetVars();
    const int ctArray = aArrayCopy.Count() - 1;

    for (int i = 0; i <= ctArray; i++) {
      (*val->GetVars())[i] = aArrayCopy[ctArray - i];
    }

  // concatenate every array entry into a string
  } ELSE_UN("$") {
    CLdsVars &aArrayValues = *val->GetVars();
    string strArray = "";

    for (int i = 0; i < aArrayValues.Count(); i++) {
      strArray += aArrayValues[i].var_valValue->Print();
    }

    val = strArray;

  } else {
    LdsUnaryError(val, tkn);
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsArrayType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  SLdsVar *pvarArrayAccess = NULL;
    
  switch (iOperation) {
    // operators
    case LOP_ADD: {
      if (val2->GetType() > EVT_FLOAT) {
        LdsBinaryError(val1, val2, tkn);
      }
        
      // expand the array
      int ctAdd = val2->GetIndex();

      while (--ctAdd >= 0) {
        val1->GetVars()->Add();
      }
    } break;

    case LOP_SUB: {
      if (val2->GetType() > EVT_FLOAT) {
        LdsBinaryError(val1, val2, tkn);
      }
        
      // shrink the array
      int ctSub = val2->GetIndex();

      while (--ctSub >= 0) {
        val1->GetVars()->Delete(val1->GetVars()->Count() - 1);
      }
    } break;

    case LOP_MUL: {
      if (val2->GetType() > EVT_FLOAT) {
        LdsBinaryError(val1, val2, tkn);
      }
        
      // copy array contents several times
      int ctOld = val1->GetVars()->Count();
      int ctNew = int(ctOld * val2->GetNumber());

      CLdsVars &aOldArray = *val1->GetVars();
      CLdsArrayType valNewArray(ctNew, 0);

      for (int i = 0; i < ctNew; i++) {
        valNewArray.aArray[i] = aOldArray[i % ctOld];
      }

      val1 = valNewArray;
    } break;

    // conditional operators
    case LOP_GT: case LOP_GOE:
    case LOP_LT: case LOP_LOE:
    case LOP_EQ: case LOP_NEQ:
      if (val2->GetType() != EVT_ARRAY) {
        LdsBinaryError(val1, val2, tkn);
      }

      switch (iOperation) {
        case LOP_GT:  val1 = int(val1->GetVars()->Count() >  val2->GetVars()->Count()); break;
        case LOP_GOE: val1 = int(val1->GetVars()->Count() >= val2->GetVars()->Count()); break;
        case LOP_LT:  val1 = int(val1->GetVars()->Count() <  val2->GetVars()->Count()); break;
        case LOP_LOE: val1 = int(val1->GetVars()->Count() <= val2->GetVars()->Count()); break;
        case LOP_EQ:  val1 = int(val1 == val2); break;
        case LOP_NEQ: val1 = int(val1 != val2); break;
      }
      break;

    // accessor
    case LOP_ACCESS: {
      if (val2->GetType() == EVT_STRING || tkn.lt_iArg >= 1) {
        // allow certain member accessors as special methods
        const string strMethod = val2->GetString();

        if (strMethod == "count") {
          val1 = val1->GetVars()->Count();
          break;

        } else {
          LdsThrow(LEX_ACCESS, "Cannot use member accessor on an array at %s", tkn.PrintPos().c_str());
        }
      }

      if (val2->GetType() > EVT_FLOAT) {
        LdsBinaryError(val1, val2, tkn);
      }

      CLdsVars aCopy = *val1->GetVars();

      int iArrayIndex = val2->GetIndex();
      int iSize = aCopy.Count();

      // out of bounds
      if (iSize <= 0) {
        LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty array at %s", tkn.PrintPos().c_str());

      } else if (iArrayIndex < 0 || iArrayIndex >= iSize) {
        LdsThrow(LEX_ARRAYOUT, "Array index '%d' is out of bounds [0, %d] at %s", iArrayIndex, iSize - 1, tkn.PrintPos().c_str());
      }

      val1 = aCopy[iArrayIndex].var_valValue;

      // get pointer to the value within the array
      pvarArrayAccess = valRef1.AccessVariable(iArrayIndex);

      // add reference index
      valRef1.AddIndex(iArrayIndex);
    } break;

    default: LdsBinaryError(val1, val2, tkn);
  }

  // copy reference indices
  CLdsValueRef valReturn(val1, valRef1.vr_pvar, pvarArrayAccess, valRef1.GetFlags());
  valReturn.vr_ariIndices = valRef1.vr_ariIndices;

  return valReturn;
};
