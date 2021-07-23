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
void CLdsObjectType::Write(class CLdsScriptEngine *pEngine, void *pStream) {
  const int ctProps = oObject.Count();

  // write object ID and if it's static
  char bStatic = oObject.bStatic;
  pEngine->_pLdsWrite(pStream, &oObject.iID, sizeof(int));
  pEngine->_pLdsWrite(pStream, &bStatic, sizeof(char));

  // write amount of keys
  pEngine->_pLdsWrite(pStream, &ctProps, sizeof(int));

  // write properties
  for (int i = 0; i < ctProps; i++) {
    pEngine->LdsWriteOneVar(pStream, oObject.aFields, i);
  }
};

// Read value from the stream
void CLdsObjectType::Read(class CLdsScriptEngine *pEngine, void *pStream, CLdsValue &val) {
  // read object ID and if it's static
  int iID = -1;
  char bStatic = false;
  pEngine->_pLdsRead(pStream, &iID, sizeof(int));
  pEngine->_pLdsRead(pStream, &bStatic, sizeof(char));

  // read amount of keys
  int ctProps = 0;
  pEngine->_pLdsRead(pStream, &ctProps, sizeof(int));

  // create a variable list
  CLdsVars aVars;

  // read properties
  for (int i = 0; i < ctProps; i++) {
    pEngine->LdsReadOneVar(pStream, aVars);
  }

  // create an object
  val = CLdsObjectType(iID, aVars, (bStatic != 0));
};
      
// Print the value
string CLdsObjectType::Print(void) {
  int ctVars = oObject.Count();
        
  if (ctVars <= 0) {
    return "{ }";
  }

  // object opening
  string strObject = "{ ";
        
  for (int iVar = 0; iVar < ctVars; iVar++) {
    if (iVar != 0) {
      // next property
      strObject += ", ";
    }
          
    // print object property
    strObject += oObject.Print(iVar);
  }
        
  // object closing
  strObject += " }";

  return strObject;
};

// Perform a unary operation
CLdsValueRef CLdsObjectType::UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn) {
  // cannot do unary operations on objects
  LdsUnaryError(valRef.vr_val, tkn);

  return valRef;
};

// Perform a binary operation
CLdsValueRef CLdsObjectType::BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) {
  // actual values and the operation
  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;

  int iOperation = tkn->GetIndex();

  // types
  const string strType1 = val1->TypeName();
  const string strType2 = val2->TypeName();

  SLdsVar *pvarField = NULL;
    
  switch (iOperation) {
    // accessor
    case LOP_ACCESS: {
      if (val2->GetType() != EVT_STRING) {
        // using array accessor on an object
        if (tkn.lt_iArg <= 0) {
          LdsThrow(LEX_OBJECTACC, "Cannot use %s as an object accessor at %s", strType2.c_str(), tkn.PrintPos().c_str());
            
        // object accessor
        } else {
          LdsThrow(LEX_EXPECTACC, "Expected a property name at %s", tkn.PrintPos().c_str());
        }
      }
        
      string strVar = val2->GetString();
      CLdsVars aCopy = val1->GetVars();

      if (aCopy.FindIndex(strVar) == -1) {
        LdsThrow(LEX_OBJECTMEM, "Property '%s' does not exist in the object at %s", strVar.c_str(), tkn.PrintPos().c_str());
      }

      SLdsVar *pvar = aCopy.Find(strVar);
      val1 = pvar->var_valValue;
        
      // get pointer to the object property
      if (valRef1.vr_pvar != NULL) {
        pvarField = valRef1.AccessVariable(strVar);
      }

      // add reference index
      valRef1.AddIndex(strVar);
    } break;
        
    default: LdsBinaryError(val1, val2, tkn);
  }
  
  // copy reference indices
  CLdsValueRef valReturn(val1, valRef1.vr_pvar, pvarField, valRef1.IsGlobal());
  valReturn.vr_ariIndices = valRef1.vr_ariIndices;

  return valReturn;
};
