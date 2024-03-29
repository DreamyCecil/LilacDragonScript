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
#include "LdsExecution.h"

extern CLdsScriptEngine *_pldsCurrent;
extern DSStack<CLdsValueRef> *_pavalStack;
extern CLdsProgram *_ppgCurrent;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent);

// Execute the compiled expression
CLdsValue CLdsScriptEngine::LdsExecute(CLdsProgram &pgProgram) {
  CActionList &acaActions = pgProgram.acaProgram;

  int iPos = 0;
  int ctActions = acaActions.Count();

  if (ctActions <= 0) {
    LdsThrow(LEX_EMPTY, "No compile actions");
  }

  // remember previous script
  CLdsProgram *ppgPrev = _ppgCurrent;
  CLdsScriptEngine *pldsPrev = _pldsCurrent;
  DSStack<CLdsValueRef> *pavalPrev = _pavalStack;
  
  _ppgCurrent = &pgProgram;
  _pldsCurrent = this;

  DSStack<CLdsValueRef> avalStack;
  _pavalStack = &avalStack;
  
  while (iPos < ctActions) {
    CCompAction &ca = SetCurrentAction(&acaActions[iPos++]);

    // set current position within the script
    LDS_iActionPos = ca.lt_iPos;
    
    switch (ca.lt_eType) {
      case LCA_VAL: Exec_Val(); break;
      case LCA_UN: Exec_Unary(); break;
      case LCA_BIN: Exec_Binary(); break;
      case LCA_GET: Exec_Get(); break;
      case LCA_CALL: Exec_Call(); break;

      default: LdsThrow(LEX_ACTION, "Can't run action %s at %s", _astrActionNames[ca.lt_eType], ca.PrintPos().c_str());
    }
  }
  
  _pldsCurrent = NULL;
  
  // restore previous script
  _ppgCurrent = ppgPrev;
  _pldsCurrent = pldsPrev;
  _pavalStack = pavalPrev;
  
  CLdsValue valResult = avalStack.Pop().vr_val;
  avalStack.Clear();
  
  return valResult;
};

// Evaluate the expression
ELdsError CLdsScriptEngine::LdsEvaluate(const string &strExpression, CLdsValue &valResult) {
  valResult = 0;

  // try to compile
  CLdsProgram pgProgram;

  ELdsError eResult = LdsCompileExpression(strExpression, pgProgram);

  if (eResult == LER_OK) {
    // try to execute
    try {
      valResult = LdsExecute(pgProgram);
      
    // plain error
    } catch (char *strError) {
      LdsErrorOut("%s (code 0x%X)\n", strError, LEX_EVALUATOR);
      return LEX_EVALUATOR;

    // LDS-specific error
    } catch (SLdsError leError) {
      LdsErrorOut("%s (code: 0x%X)\n", leError.le_strMessage.c_str(), leError.le_eError);
      return leError.le_eError;
    }
  }

  return eResult;
};

// Evaluate compiled expression
ELdsError CLdsScriptEngine::LdsEvaluateCompiled(CLdsProgram &pgProgram, CLdsValue &valResult) {
  valResult = 0;

  // try to execute
  try {
    valResult = LdsExecute(pgProgram);

  // plain error
  } catch (char *strError) {
    LdsErrorOut("%s (code 0x%X)\n", strError, LEX_EVALUATOR);
    return LEX_EVALUATOR;

  // LDS-specific error
  } catch (SLdsError leError) {
    LdsErrorOut("%s (code: 0x%X)\n", leError.le_strMessage.c_str(), leError.le_eError);
    return leError.le_eError;
  }

  return LER_OK;
};
