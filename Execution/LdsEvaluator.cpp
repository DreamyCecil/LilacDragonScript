#include "../LdsScriptEngine.h"
#include "LdsExecution.h"

extern CLdsScriptEngine *_pldsCurrent;
extern CDStack<SLdsValueRef> *_pavalStack;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent);

// Execute the compiled expression
SLdsValue CLdsScriptEngine::LdsExecute(CActionList &acaActions) {
  int iPos = 0;
  int ctActions = acaActions.Count();

  if (ctActions <= 0) {
    LdsThrow(LEX_EMPTY, "No compile actions");
  }
  
  // remember previous stack
  CDStack<SLdsValueRef> *pavalStackPrev = _pavalStack;
  
  CDStack<SLdsValueRef> avalStack;
  _pavalStack = &avalStack;
  
  // use this engine for the thread
  _pldsCurrent = this;
  
  while (iPos < ctActions) {
    CCompAction &ca = SetCurrentAction(&acaActions[iPos++]);
    
    switch (ca.lt_eType) {
      case LCA_VAL: Exec_Val(); break;
      case LCA_UN: Exec_Unary(); break;
      case LCA_BIN: Exec_Binary(); break;
      case LCA_GET: Exec_Get(); break;
      case LCA_CALL: Exec_Call(); break;

      default: LdsThrow(LEX_ACTION, "Can't run action %d at %s", ca.lt_eType, ca.PrintPos().c_str());
    }
  }
  
  _pldsCurrent = NULL;
  
  // restore previous stack
  _pavalStack = pavalStackPrev;
  
  SLdsValue valResult = avalStack.Pop().val;
  avalStack.Clear();
  
  return valResult;
};

// Evaluate the expression
ELdsError CLdsScriptEngine::LdsEvaluate(string strExpression, SLdsValue &valResult) {
  valResult = 0.0f;

  // try to compile
  CActionList acaActions;

  ELdsError eResult = LdsCompileExpression(strExpression, acaActions);

  if (eResult == LER_OK) {
    // try to execute
    try {
      valResult = LdsExecute(acaActions);
      
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
ELdsError CLdsScriptEngine::LdsEvaluateCompiled(CActionList acaActions, SLdsValue &valResult) {
  valResult = 0.0f;

  // try to execute
  try {
    valResult = LdsExecute(acaActions);

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
