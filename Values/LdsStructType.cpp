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
  // cannot do unary operations on structures
  LdsThrow(LEX_UNARY, "Cannot perform a unary operation on a structure at %s", ca.PrintPos().c_str());

  return valRef;
};

// Perform a binary operation
CLdsValueRef CLdsStructType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, CCompAction &ca) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = ca->GetIndex();

  // types
  const string strType1 = val1->TypeName("a number", "a string", "an array", "a structure");
  const string strType2 = val2->TypeName("a number", "a string", "an array", "a structure");

  CLdsValue *pvalStructAccess = NULL;

  // structure variable properties
  string strStructVar = valRef1.vr_strVar;
  bool bConstVar = false;
    
  switch (iOperation) {
    // accessor
    case LOP_ACCESS: {
      if (val2->GetType() != EVT_STRING) {
        // using array accessor on a structure
        if (ca.lt_iArg <= 0) {
          LdsThrow(LEX_STRUCTACC, "Cannot use %s as a structure accessor at %s", strType2.c_str(), ca.PrintPos().c_str());
            
        // structure accessor
        } else {
          LdsThrow(LEX_EXPECTACC, "Expected a structure variable name at %s", ca.PrintPos().c_str());
        }
      }
        
      string strVar = val2->GetString();
        
      // direct 'val1 = val1.sStruct' empties its own struct before getting a value from it
      CLdsStruct sCopy = val1->GetStruct();

      if (sCopy.FindIndex(strVar) == -1) {
        LdsThrow(LEX_STRUCTVAR, "Variable '%s' does not exist in the structure at %s", strVar.c_str(), ca.PrintPos().c_str());
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
        
    default: LdsThrow(LEX_BINARY, "Cannot perform a binary operation %d on %s and %s at %s", ca->GetIndex(), strType1.c_str(), strType2.c_str(), ca.PrintPos().c_str());
  }

  return CLdsValueRef(val1, valRef1.vr_pvar, pvalStructAccess, valRef1.vr_strVar, strStructVar, bConstVar, valRef1.IsGlobal());
};
