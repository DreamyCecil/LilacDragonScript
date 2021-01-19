#include "../LdsScriptEngine.h"
#include <math.h>

extern CLdsThread *_psthCurrent;

// Engine that handles current thread
extern CLdsScriptEngine *_pldsCurrent = NULL;

// Execution stack
extern CDStack<SLdsValueRef> *_pavalStack = NULL;

// Current action
static CCompAction *_ca = NULL;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent) {
  _ca = pcaCurrent;
  return *pcaCurrent;
};

// Get index of a local variable
int GetLocalVar(void) {
  string strName = _ca->lt_valValue;
  
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
    int iContainer = int(_ca->lt_valValue);
    
    // array
    if (iContainer == 0) {
      SLdsValue valArray = SLdsValue(ctValues, 0.0f);
      
      CDArray<SLdsValue> avalArray;
      avalArray.New(ctValues);
      
      // get array entries
      for (int iPopVal = ctValues-1; iPopVal >= 0; iPopVal--) {
        ThreadOut(false);
        avalArray[iPopVal] = _pavalStack->Pop().val;
      }
      
      // add them in the array
      for (int iArrayVal = 0; iArrayVal < ctValues; iArrayVal++) {
        valArray.aArray[iArrayVal] = avalArray[iArrayVal];
      }
      
      _pavalStack->Push(SLdsValueRef(valArray));
      ThreadOut(true);
      
    // structure
    } else {
      CLdsVarMap mapVars;
      
      // get structure variables
      for (int iVar = 0; iVar < ctValues; iVar++) {
        // variable name
        string strVar = _pavalStack->Pop();
        // constant variable
        int iConst = _pavalStack->Pop();
        // value
        SLdsValue val = _pavalStack->Pop().val;
        
        // add the variable
        mapVars[strVar] = SLdsVar(val, (iConst >= 1));
      }
      
      // fill the structure
      SLdsValue valStruct = SLdsValue(-1, mapVars, (iContainer > 1));
      
      _pavalStack->Push(SLdsValueRef(valStruct));
      ThreadOut(true);
    }

  // value
  } else {
    _pavalStack->Push(SLdsValueRef(_ca->lt_valValue));
    ThreadOut(true);
  }
};

// Unary operations
void Exec_Unary(void) {
  ThreadOut(false);
  SLdsValueRef valRef = _pavalStack->Pop();

  if (valRef.val.eType != EVT_FLOAT) {
    LdsThrow(LEX_UNARY, "Cannot perform unary operation on a value that isn't a number at %s", _ca->PrintPos().c_str());
  }

  switch ((int)_ca->lt_valValue) {
    case UOP_NEGATE:
      valRef.val = -valRef.val.fValue;
      ThreadOut(true);
      break;

    // TODO: Make string inversion
    case UOP_INVERT: {
      bool bInvert = (valRef.val.fValue >= 0.5f);
      valRef.val = !bInvert;
      ThreadOut(true);
    } break;

    case UOP_BINVERT: {
      int iInvert = (int)valRef.val.fValue;
      valRef.val = ~iInvert;
      ThreadOut(true);
    } break;
  }

  // push back into the stack
  _pavalStack->Push(valRef);
};

// Binary operations
void Exec_Binary(void) {
  SLdsValueRef valRef2 = _pavalStack->Pop();
  SLdsValueRef valRef1 = _pavalStack->Pop();

  SLdsValue val1 = valRef1.val;
  SLdsValue val2 = valRef2.val;
  
  // types
  ELdsValueType eType1 = val1.eType;
  ELdsValueType eType2 = val2.eType;
  
  string strType1 = val1.TypeName("a number", "a string", "an array", "a structure");
  string strType2 = val2.TypeName("a number", "a string", "an array", "a structure");
    
  int iOperation = _ca->lt_valValue.fValue;
  
  // structure operations
  if (eType1 == EVT_STRUCT) {
    SLdsValue *pvalStructAccess = NULL;

    // structure variable properties
    string strStructVar = valRef1.strVar;
    bool bConstVar = false;
    
    switch (iOperation) {
      // accessor
      case LOP_ACCESS: {
        if (eType2 != EVT_STRING) {
          // using array accessor on a structure
          if (_ca->lt_iArg <= 0) {
            LdsThrow(LEX_STRUCTACC, "Cannot use %s as a structure accessor at %s", strType2.c_str(), _ca->PrintPos().c_str());
            
          // structure accessor
          } else {
            LdsThrow(LEX_EXPECTACC, "Expected a structure variable name at %s", _ca->PrintPos().c_str());
          }
        }
        
        string strVar = val2;
        
        // direct 'val1 = val1.sStruct' empties its own struct before getting a value from it
        SLdsStruct sCopy = val1.sStruct;

        if (sCopy.FindIndex(strVar) == -1) {
          LdsThrow(LEX_STRUCTVAR, "Variable '%s' does not exist in the structure at %s", strVar.c_str(), _ca->PrintPos().c_str());
        }
        
        val1 = sCopy[strVar];
        
        // get pointer to the value within the struct
        if (valRef1.pvar != NULL) {
          if (valRef1.pvalAccess != NULL) {
            pvalStructAccess = &valRef1.pvalAccess->sStruct[strVar];
          } else {
            pvalStructAccess = &valRef1.pvar->var_valValue.sStruct[strVar];
          }

          // get variable name and check for const
          strStructVar = strVar;
          bConstVar = (sCopy.mapVars[strVar].var_bConst > 0);
        }
      } break;
        
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", (int)_ca->lt_valValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }
    
    _pavalStack->Push(SLdsValueRef(val1, valRef1.pvar, pvalStructAccess, valRef1.strVar, strStructVar, bConstVar));
    return;
  }

  // array operations
  if (eType1 == EVT_ARRAY) {
    SLdsValue *pvalArrayAccess = NULL;
    
    switch (iOperation) {
      // TODO: Add multiplication operator to copy array contents multiple times
      // operators
      case LOP_ADD: {
        if (eType2 != EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot add %s to an array at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // expand the array
        int ctResize = (val1.aArray.Count() + val2.fValue);
        val1.aArray.Resize(ctResize);
      } break;
        
      case LOP_SUB: {
        if (eType2 != EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot subtract %s from an array at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // shrink the array
        int ctResize = (val1.aArray.Count() - val2.fValue);
        val1.aArray.Resize(ctResize);
      } break;
      
      // conditional operators
      case LOP_GT: case LOP_GOE:
      case LOP_LT: case LOP_LOE:
      case LOP_EQ: case LOP_NEQ:
        if (eType2 != EVT_ARRAY) {
          LdsThrow(LEX_BINARY, "Cannot compare an array with %s at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        switch (iOperation) {
          case LOP_GT:  val1 = float(val1.aArray.Count() > val2.aArray.Count()); break;
          case LOP_GOE: val1 = float(val1.aArray.Count() >= val2.aArray.Count()); break;
          case LOP_LT:  val1 = float(val1.aArray.Count() < val2.aArray.Count()); break;
          case LOP_LOE: val1 = float(val1.aArray.Count() <= val2.aArray.Count()); break;
          case LOP_EQ:  val1 = float(val1 == val2); break;
          case LOP_NEQ: val1 = float(val1 != val2); break;
        }
        break;
      
      // accessor
      case LOP_ACCESS: {
        if (_ca->lt_iArg >= 1) {
          LdsThrow(LEX_BINARY, "Cannot use structure accessor on the array at %s", _ca->PrintPos().c_str());
        }
        
        if (eType2 != EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot use %s as an array accessor at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // direct 'val1 = val1.aArray' empties its own array before getting a value from it
        CDArray<SLdsValue> aCopy = val1.aArray;
        
        int iArrayIndex = val2;
        int iSize = aCopy.Count();
    
        // out of bounds
        if (iSize <= 0) {
          LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty array at %s", _ca->PrintPos().c_str());

        } else if (iArrayIndex < 0 || iArrayIndex >= iSize) {
          LdsThrow(LEX_ARRAYOUT, "Array index '%d' is out of bounds [0, %d] at %s", iArrayIndex, iSize-1, _ca->PrintPos().c_str());
        }
        
        val1 = aCopy[iArrayIndex];
        
        // get pointer to the value within the array
        if (valRef1.pvar != NULL) {
          if (valRef1.pvalAccess != NULL) {
            pvalArrayAccess = &valRef1.pvalAccess->aArray[iArrayIndex];
          } else {
            pvalArrayAccess = &valRef1.pvar->var_valValue.aArray[iArrayIndex];
          }
        }
      } break;
      
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", (int)_ca->lt_valValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }
    
    _pavalStack->Push(SLdsValueRef(val1, valRef1.pvar, pvalArrayAccess, valRef1.strVar, valRef1.strRef, valRef1.bConst));
    ThreadOut(true);
    return;
  }

  bool bStr1 = (eType1 == EVT_STRING);
  bool bStr2 = (eType2 == EVT_STRING);

  // strings are involved
  if (bStr1 || bStr2) {
    switch (iOperation) {
      // append strings
      case LOP_ADD:
        val1 = val1.Print() + val2.Print();
        break;

      // remove characters from the end
      case LOP_SUB: {
        if (!bStr1) {
          LdsThrow(LEX_BINARY, "Cannot subtract a string from a number at %s", _ca->PrintPos().c_str());
        }
        if (bStr2) {
          LdsThrow(LEX_BINARY, "Cannot subtract a string from a string at %s", _ca->PrintPos().c_str());
        }

        string str = val1.strValue;
        int ctStr = strlen(str.c_str());
        int ctSub = val2.fValue;

        val1 = str.substr(0, ctStr - ctSub);
      } break;

      // copy the same string multiple times
      case LOP_MUL: {
        if (!bStr1) {
          LdsThrow(LEX_BINARY, "Cannot multiply a number by a string at %s", _ca->PrintPos().c_str());
        }
        if (bStr2) {
          LdsThrow(LEX_BINARY, "Cannot multiply a string by a string at %s", _ca->PrintPos().c_str());
        }

        string strMul = "";

        for (int iCopy = 0; iCopy < val2.fValue; iCopy++) {
          strMul += val1.strValue;
        }

        val1 = strMul;
      } break;

      // compare strings
      case LOP_EQ: {
        bool bResult = false;

        // compare length
        if (!bStr1 && bStr2) {
          bResult = int(val1.fValue) == strlen(val2.strValue);
        } else if (bStr1 && !bStr2) {
          bResult = int(val2.fValue) == strlen(val1.strValue);

        // compare string contents
        } else {
          bResult = (val1 == val2);
        }

        val1 = (float)bResult;
      } break;

      case LOP_NEQ: {
        bool bResult = false;

        // compare length
        if (!bStr1 && bStr2) {
          bResult = int(val1.fValue) != strlen(val2.strValue);
        } else if (bStr1 && !bStr2) {
          bResult = int(val2.fValue) != strlen(val1.strValue);

        // compare string contents
        } else {
          bResult = (val1 != val2);
        }

        val1 = (float)bResult;
      } break;

      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", (int)_ca->lt_valValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }

  } else {
    // get numbers
    float fNum1 = val1.fValue;
    float fNum2 = val2.fValue;

    switch (iOperation) {
      // operators
      case LOP_ADD: fNum1 += fNum2; break;
      case LOP_SUB: fNum1 -= fNum2; break;
      case LOP_MUL: fNum1 *= fNum2; break;
      case LOP_DIV: fNum1 /= fNum2; break;
    
      case LOP_FMOD:
        if (fNum2 != 0.0f) {
          fNum1 = fmodf(fNum1, fNum2);
        } else {
          fNum1 = 0.0f;
        }
        break;
      
      case LOP_IDIV:
        if (fNum2 != 0.0f) {
          fNum1 = int(fNum1 / fNum2);
        } else {
          fNum1 = 0.0f;
        }
        break;
        
      // bitwise operators
      case LOP_SH_L: fNum1 = int(fNum1) << int(fNum2); break;
      case LOP_SH_R: fNum1 = int(fNum1) >> int(fNum2); break;
      case LOP_B_AND: fNum1 = int(fNum1) & int(fNum2); break;
      case LOP_B_XOR: fNum1 = int(fNum1) ^ int(fNum2); break;
      case LOP_B_OR:  fNum1 = int(fNum1) | int(fNum2); break;

      // conditional operators
      case LOP_AND: fNum1 = (fNum1 >= 0.5f) && (fNum2 >= 0.5f); break;
      case LOP_OR:  fNum1 = (fNum1 >= 0.5f) || (fNum2 >= 0.5f); break;
      case LOP_XOR: fNum1 = ((fNum1 >= 0.5f) ^ (fNum2 >= 0.5f)) > 0; break;

      case LOP_GT:  fNum1 = (fNum1 > fNum2); break;
      case LOP_GOE: fNum1 = (fNum1 >= fNum2); break;
      case LOP_LT:  fNum1 = (fNum1 < fNum2); break;
      case LOP_LOE: fNum1 = (fNum1 <= fNum2); break;
      case LOP_EQ:  fNum1 = (fNum1 == fNum2); break;
      case LOP_NEQ: fNum1 = (fNum1 != fNum2); break;
      
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d at %s", (int)_ca->lt_valValue, _ca->PrintPos().c_str());
    }

    // assign a number
    val1 = fNum1;
  }
  
  _pavalStack->Push(SLdsValueRef(val1));
  ThreadOut(true);
};

// Get variable value
void Exec_Get(void) {
  SLdsVar *pvar = NULL;
  string strName = _ca->lt_valValue;
        
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", _ca->lt_valValue.strValue, _ca->PrintPos().c_str());
  }
  
  SLdsValue *pvalRef = &pvar->var_valValue;
  
  _pavalStack->Push(SLdsValueRef(*pvalRef, pvar, NULL, strName, strName, false));
  ThreadOut(true);
};

// Set variable value
void Exec_Set(void) {
  SLdsVar *pvar = NULL;
  string strName = _ca->lt_valValue;
  
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", _ca->lt_valValue.strValue, _ca->PrintPos().c_str());
  }
  
  // check if it's a constant
  if (pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", strName.c_str(), _ca->PrintPos().c_str());
  }
  
  ThreadOut(false);
  SLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.pvalAccess != NULL && pvar == valRef.pvar) {
    // set value within the array
    *valRef.pvalAccess = valRef.val;
    
  } else {
    // set value to the variable
    pvar->var_valValue = valRef.val;
  }
  
  // constant variables
  pvar->SetConst();
};

// Get local variable value
void Exec_GetLocal(void) {
  int iLocal = GetLocalVar();
  CLdsVarMap &mapLocals = _psthCurrent->sth_mapLocals;
  
  string strName = _ca->lt_valValue;

  SLdsVar *pvar = &mapLocals.GetValue(iLocal);
  SLdsValue *pvalLocal = &pvar->var_valValue;
  
  _pavalStack->Push(SLdsValueRef(*pvalLocal, pvar, NULL, strName, strName, false));
  ThreadOut(true);
};

// Set local variable value
void Exec_SetLocal(void) {
  int iLocal = GetLocalVar();
  CLdsVarMap &mapLocals = _psthCurrent->sth_mapLocals;
  SLdsVar *pvar = &mapLocals.GetValue(iLocal);
  
  // check if it's a constant
  if (pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", _ca->lt_valValue.strValue, _ca->PrintPos().c_str());
  }
  
  ThreadOut(false);
  SLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.pvalAccess != NULL && pvar == valRef.pvar) {
    // set value within the array
    *valRef.pvalAccess = _pavalStack->Pop().val;
    
  } else {
    // set value to the variable
    pvar->var_valValue = valRef.val;
  }
  
  // constant variables
  pvar->SetConst();
};

// Set variable through the accessor
void Exec_SetAccessor(void) {
  SLdsValueRef valRef = _pavalStack->Pop();
  
  // check for an array accessor
  if (valRef.pvalAccess == NULL) {
    LdsThrow(LEX_ACCESS, "Trying to set value to an accessor with no accessor reference at %s", _ca->PrintPos().c_str());
  }

  // constant reference
  if (valRef.pvar->var_bConst > 1) {
    LdsThrow(LEX_CONST, "Cannot reassign value of a constant variable '%s' at %s", valRef.strVar.c_str(), _ca->PrintPos().c_str());
  }

  // check for constants
  if (valRef.bConst) {
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", valRef.strRef.c_str(), _ca->PrintPos().c_str());
  }
  
  // set value within the array
  *valRef.pvalAccess = _pavalStack->Pop().val;
};

// Function call
void Exec_Call(void) {
  // make a list of arguments
  CLdsValueList avalArgs = MakeValueList(*_pavalStack, _ca->lt_iArg);

  // call the function
  SLdsValueRef valValue = _pldsCurrent->CallFunction(_ca->lt_valValue, avalArgs);
  
  _pavalStack->Push(valValue);
  ThreadOut(true);
};
