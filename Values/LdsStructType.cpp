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
void CLdsStructType::Write(CLdsWriteFunc pWriteFunc) {
  
};

// Read value from the stream
void CLdsStructType::Read(CLdsReadFunc pWriteFunc) {
  
};
      
// Print the value
string CLdsStructType::Print(void) {
  int ctVars = sStruct.Count();
        
  if (ctVars <= 0) {
    return "{ }";
  }

  // structure opening
  string strStruct = "{ ";
        
  for (int iVar = 0; iVar < ctVars; iVar++) {
    if (iVar != 0) {
      // next variable
      strStruct += ", ";
    }
          
    // print structure variable
    strStruct += sStruct.Print(iVar);
  }
        
  // structure closing
  strStruct += " }";

  return strStruct;
};

// Perform a unary operation
CLdsValueRef CLdsStructType::UnaryOp(CLdsValueRef &valRef, CCompAction &ca) {
  // actual value and the operation
  CLdsValue val = valRef.vr_val;
  int iOperation = ca->GetIndex();

  switch (iOperation) {
    // get pointer
    case UOP_POINTER: {
      val = valRef.ToPointer();
    } break;

    default: LdsThrow(LEX_UNARY, "Cannot perform a unary operation %d on a structure at %s", ca->GetIndex(), ca.PrintPos().c_str());
  }

  return CLdsValueRef(val);
};

// Perform a binary operation
CLdsValueRef CLdsStructType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  // types
  const string strType1 = val1->TypeName();
  const string strType2 = val2->TypeName();

  CLdsValue *pvalStructAccess = NULL;

  // structure variable properties
  string strStructVar = valRef1.vr_strVar;
  bool bConstVar = false;
    
  switch (iOperation) {
    // accessor
    case LOP_ACCESS: {
      if (val2->GetType() != EVT_STRING) {
        // using array accessor on a structure
        if (tkn.lt_iArg <= 0) {
          LdsThrow(LEX_STRUCTACC, "Cannot use %s as a structure accessor at %s", strType2.c_str(), tkn.PrintPos().c_str());
            
        // structure accessor
        } else {
          LdsThrow(LEX_EXPECTACC, "Expected a structure variable name at %s", tkn.PrintPos().c_str());
        }
      }
        
      string strVar = val2->GetString();
        
      // direct 'val1 = val1->GetStruct()' empties its own struct before getting a value from it
      CLdsStruct sCopy = val1->GetStruct();

      if (sCopy.FindIndex(strVar) == -1) {
        LdsThrow(LEX_STRUCTVAR, "Variable '%s' does not exist in the structure at %s", strVar.c_str(), tkn.PrintPos().c_str());
      }

      val1 = sCopy[strVar];
        
      // get pointer to the value within the structure
      if (valRef1.vr_pvar != NULL) {
        pvalStructAccess = valRef1.GetValue(strVar);

        // get variable name and check for const
        strStructVar = strVar;
        bConstVar = (sCopy.mapVars[strVar].var_bConst > 0);
      }

      // add reference index
      valRef1.AddIndex(strVar);
    } break;
        
    default: LdsBinaryError(val1, val2, tkn);
  }
  
  // copy reference indices
  CLdsValueRef valReturn(val1, valRef1.vr_pvar, pvalStructAccess, valRef1.vr_strVar, strStructVar, bConstVar, valRef1.IsGlobal());
  valReturn.vr_ariIndices = valRef1.vr_ariIndices;

  return valReturn;
};
