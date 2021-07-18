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
extern CDStack<CLdsValueRef> *_pavalStack = NULL;

// Current action
static CCompAction *_ca = NULL;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent) {
  _ca = pcaCurrent;
  return *pcaCurrent;
};

// Get index of a local variable
int GetLocalVar(void) {
  string strName = (*_ca)->GetString();
  
  CLdsVarMap &mapLocals = _psthCurrent->sth_mapLocals;
  int iLocal = -1;
  
  // prioritize inline locals
  if (_psthCurrent->sth_aicCalls.Count() > 0) {
    SLdsInlineCall &ic = _psthCurrent->sth_aicCalls.Top();
    
    string strInline = ic.VarName(strName);
    iLocal = mapLocals.FindKeyIndex(strInline);
  }
  
  // check global locals
  if (iLocal == -1) {
    iLocal = mapLocals.FindKeyIndex(strName);
  }
  
  // doesn't exist
  if (iLocal == -1) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  return iLocal;
};

// Values
void Exec_Val(void) {
  int ctValues = _ca->lt_iArg;
  
  if (ctValues >= 0) {
    int iContainer = (*_ca)->GetIndex();
    
    // array
    if (iContainer == 0) {
      CLdsValue valArray = CLdsValue(ctValues, 0.0f);
      
      CLdsArray avalArray;
      avalArray.New(ctValues);
      
      // get array entries
      for (int iPopVal = ctValues - 1; iPopVal >= 0; iPopVal--) {
        avalArray[iPopVal] = _pavalStack->Pop().vr_val;
      }
      
      // add them in the array
      for (int iArrayVal = 0; iArrayVal < ctValues; iArrayVal++) {
        valArray->GetArray()[iArrayVal] = avalArray[iArrayVal];
      }
      
      _pavalStack->Push(CLdsValueRef(valArray));
      
    // structure
    } else {
      CLdsVarMap mapVars;
      
      // get structure variables
      for (int iVar = 0; iVar < ctValues; iVar++) {
        // variable name
        string strVar = _pavalStack->Pop().vr_val->GetString();
        // constant variable
        bool bConst = _pavalStack->Pop().vr_val->IsTrue();
        // value
        CLdsValue val = _pavalStack->Pop().vr_val;
        
        // add the variable
        mapVars[strVar] = SLdsVar(val, bConst);
      }
      
      // fill the structure
      CLdsValue valStruct = CLdsValue(-1, mapVars, (iContainer > 1));
      _pavalStack->Push(CLdsValueRef(valStruct));
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
  SLdsVar *pvar = NULL;
  string strName = (*_ca)->GetString();
        
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  CLdsValue *pvalRef = &pvar->var_valValue;
  
  _pavalStack->Push(CLdsValueRef(*pvalRef, pvar, NULL, strName, strName, false, true));
};

// Set variable value
void Exec_Set(void) {
  SLdsVar *pvar = NULL;
  string strName = (*_ca)->GetString();
  
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  // check if it's a constant
  if (pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  CLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.vr_pvalAccess != NULL && pvar == valRef.vr_pvar) {
    // set value within the array
    *valRef.vr_pvalAccess = valRef.vr_val;
    
  } else {
    // set value to the variable
    pvar->var_valValue = valRef.vr_val;
  }
  
  // constant variables
  pvar->SetConst();
};

// Get local variable value
void Exec_GetLocal(void) {
  int iLocal = GetLocalVar();
  CLdsVarMap &mapLocals = _psthCurrent->sth_mapLocals;
  
  string strName = (*_ca)->GetString();

  SLdsVar *pvar = &mapLocals.GetValue(iLocal);
  CLdsValue *pvalLocal = &pvar->var_valValue;
  
  _pavalStack->Push(CLdsValueRef(*pvalLocal, pvar, NULL, strName, strName, false, false));
};

// Set local variable value
void Exec_SetLocal(void) {
  int iLocal = GetLocalVar();
  CLdsVarMap &mapLocals = _psthCurrent->sth_mapLocals;
  SLdsVar *pvar = &mapLocals.GetValue(iLocal);
  
  // check if it's a constant
  if (pvar->var_bConst > 1) {
    string strName = (*_ca)->GetString();
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  CLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.vr_pvalAccess != NULL && pvar == valRef.vr_pvar) {
    // set value within the array
    *valRef.vr_pvalAccess = _pavalStack->Pop().vr_val;
    
  } else {
    // set value to the variable
    pvar->var_valValue = valRef.vr_val;
  }
  
  // constant variables
  pvar->SetConst();
};

// Set variable through the accessor
void Exec_SetAccessor(void) {
  CLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.vr_pvalAccess == NULL) {
    LdsThrow(LEX_ACCESS, "Trying to set value to an accessor with no accessor reference at %s", _ca->PrintPos().c_str());
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
  CLdsValueList avalArgs = MakeValueList(*_pavalStack, _ca->lt_iArg);

  // call the function
  CLdsValueRef valValue = _pldsCurrent->CallFunction(_ca, avalArgs);
  
  _pavalStack->Push(valValue);
};
