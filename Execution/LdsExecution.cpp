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

extern CLdsThread *_psthCurrent;

// Engine that handles current thread
extern CLdsScriptEngine *_pldsCurrent = NULL;

// Execution stack
extern DSStack<CLdsValueRef> *_pavalStack = NULL;

// Current action
static CCompAction *_ca = NULL;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent) {
  _ca = pcaCurrent;
  return *pcaCurrent;
};

// Get index of a local variable
SLdsVar *GetLocalVar(void) {
  string strName = (*_ca)->GetString();
  
  CLdsVars &aLocals = _psthCurrent->sth_aLocals;
  SLdsVar *pvarLocal = NULL;
  
  // prioritize inline locals
  if (_psthCurrent->sth_aicCalls.Count() > 0) {
    SLdsInlineCall &ic = _psthCurrent->sth_aicCalls.Top();
    
    string strInline = ic.VarName(strName);
    pvarLocal = aLocals.Find(strInline);
  }
  
  // check global locals
  if (pvarLocal == NULL) {
    pvarLocal = aLocals.Find(strName);
  }
  
  // doesn't exist
  if (pvarLocal == NULL) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  return pvarLocal;
};

// Values
void Exec_Val(void) {
  int ctValues = _ca->lt_iArg;
  
  if (ctValues >= 0) {
    int iContainer = (*_ca)->GetIndex();
    
    // array
    if (iContainer == 0) {
      CLdsArray aArray;
      aArray.New(ctValues);
      
      // get array entries
      for (int iPopVal = ctValues - 1; iPopVal >= 0; iPopVal--) {
        aArray[iPopVal] = _pavalStack->Pop().vr_val;
      }
      
      _pavalStack->Push(CLdsValueRef(CLdsArrayType(aArray)));
      
    // structure
    } else {
      CLdsVars aVars;
      
      // get structure variables
      for (int iVar = 0; iVar < ctValues; iVar++) {
        // variable name
        string strVar = _pavalStack->Pop().vr_val->GetString();
        // constant variable
        bool bConst = _pavalStack->Pop().vr_val->IsTrue();
        // value
        CLdsValue val = _pavalStack->Pop().vr_val;
        
        // add the variable
        aVars.Add(SLdsVar(strVar, val, bConst));
      }
      
      // fill the structure
      _pavalStack->Push(CLdsValueRef(CLdsStructType(-1, aVars, (iContainer > 1))));
    }

  // value
  } else {
    _pavalStack->Push(CLdsValueRef(_ca->lt_valValue));
  }
};

// Unary operations
void Exec_Unary(void) {
  CLdsValueRef valRef = _pavalStack->Pop();
  
  // value to return
  valRef = valRef.vr_val->UnaryOp(valRef, *_ca);
  _pavalStack->Push(valRef);
};

// Binary operations
void Exec_Binary(void) {
  // values for the operation
  CLdsValueRef valRef2 = _pavalStack->Pop();
  CLdsValueRef valRef1 = _pavalStack->Pop();
  
  // value to return
  valRef1 = valRef1.vr_val->BinaryOp(valRef1, valRef2, *_ca);
  _pavalStack->Push(valRef1);
};

// Get variable value
void Exec_Get(void) {
  // try to get the variable
  string strName = (*_ca)->GetString();
  SLdsVar *pvar = _pldsCurrent->_aLdsVariables.Find(strName);

  // no variable
  if (pvar == NULL) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }

  CLdsValue *pvalRef = &pvar->var_valValue;

  _pavalStack->Push(CLdsValueRef(*pvalRef, pvar, NULL, strName, strName, CLdsValueRef::VRF_GLOBAL));
};

// Set variable value
void Exec_Set(void) {
  // try to get the variable
  string strName = (*_ca)->GetString();
  SLdsVar *pvar = _pldsCurrent->_aLdsVariables.Find(strName);

  // no variable
  if (pvar == NULL) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }

  // check if it's a constant
  if (pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", strName.c_str(), _ca->PrintPos().c_str());
  }

  // set value to the variable
  pvar->var_valValue = _pavalStack->Pop().vr_val;
  pvar->SetConst();
};

// Get local variable value
void Exec_GetLocal(void) {
  SLdsVar *pvar = GetLocalVar();
  string strName = (*_ca)->GetString();

  CLdsValue *pvalLocal = &pvar->var_valValue;
  
  _pavalStack->Push(CLdsValueRef(*pvalLocal, pvar, NULL, strName, strName, 0));
};

// Set local variable value
void Exec_SetLocal(void) {
  SLdsVar *pvar = GetLocalVar();
  
  // check if it's a constant
  if (pvar->var_bConst > 1) {
    string strName = (*_ca)->GetString();
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  // set value to the variable
  pvar->var_valValue = _pavalStack->Pop().vr_val;
  pvar->SetConst();
};

// Set variable through the accessor
void Exec_SetAccessor(void) {
  CLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.vr_pvalAccess == NULL) {
    LdsThrow(LEX_NOACCESS, "Cannot set value through an accessor at %s", _ca->PrintPos().c_str());
  }

  // constant reference
  if (valRef.vr_pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign value of a constant variable '%s' at %s", valRef.vr_strVar.c_str(), _ca->PrintPos().c_str());
  }

  // check for constants
  if (valRef.IsConst()) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", valRef.vr_strRef.c_str(), _ca->PrintPos().c_str());
  }
  
  // set value within the array
  *valRef.vr_pvalAccess = _pavalStack->Pop().vr_val;
};

// Function call
void Exec_Call(void) {
  // make a list of arguments
  CLdsArray aArgs = MakeValueList(*_pavalStack, _ca->lt_iArg);

  // call the function
  CLdsValueRef valValue = _pldsCurrent->CallFunction(_ca, aArgs);
  
  _pavalStack->Push(valValue);
};
