#include "../LdsScriptEngine.h"
#include <math.h>

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
  string strName = _ca->lt_valValue.strValue;
  
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
    int iContainer = _ca->lt_valValue.iValue;
    
    // array
    if (iContainer == 0) {
      CLdsValue valArray = CLdsValue(ctValues, 0.0f);
      
      CLdsArray avalArray;
      avalArray.New(ctValues);
      
      // get array entries
      for (int iPopVal = ctValues-1; iPopVal >= 0; iPopVal--) {
        avalArray[iPopVal] = _pavalStack->Pop().vr_val;
      }
      
      // add them in the array
      for (int iArrayVal = 0; iArrayVal < ctValues; iArrayVal++) {
        valArray.aArray[iArrayVal] = avalArray[iArrayVal];
      }
      
      _pavalStack->Push(CLdsValueRef(valArray));
      
    // structure
    } else {
      CLdsVarMap mapVars;
      
      // get structure variables
      for (int iVar = 0; iVar < ctValues; iVar++) {
        // variable name
        string strVar = _pavalStack->Pop().vr_val.strValue;
        // constant variable
        int iConst = _pavalStack->Pop().vr_val.iValue;
        // value
        CLdsValue val = _pavalStack->Pop().vr_val;
        
        // add the variable
        mapVars[strVar] = SLdsVar(val, (iConst >= 1));
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

  if (valRef.vr_val.eType > EVT_FLOAT) {
    LdsThrow(LEX_UNARY, "Cannot perform unary operation on a value that isn't a number at %s", _ca->PrintPos().c_str());
  }

  switch (_ca->lt_valValue.iValue) {
    case UOP_NEGATE:
      valRef.vr_val = -valRef.vr_val.GetNumber();
      break;

    // TODO: Make string inversion
    case UOP_INVERT: {
      bool bInvert = (valRef.vr_val.GetIndex() > 0);
      valRef.vr_val = !bInvert;
    } break;

    case UOP_BINVERT: {
      int iInvert = valRef.vr_val.GetIndex();
      valRef.vr_val = ~iInvert;
    } break;
  }

  // push back into the stack
  _pavalStack->Push(valRef);
};

// Binary operations
void Exec_Binary(void) {
  CLdsValueRef valRef2 = _pavalStack->Pop();
  CLdsValueRef valRef1 = _pavalStack->Pop();

  CLdsValue val1 = valRef1.vr_val;
  CLdsValue val2 = valRef2.vr_val;
  
  // types
  ELdsValueType eType1 = val1.eType;
  ELdsValueType eType2 = val2.eType;
  
  string strType1 = val1.TypeName("a number", "a string", "an array", "a structure");
  string strType2 = val2.TypeName("a number", "a string", "an array", "a structure");
    
  int iOperation = _ca->lt_valValue.iValue;
  
  // structure operations
  if (eType1 == EVT_STRUCT) {
    CLdsValue *pvalStructAccess = NULL;

    // structure variable properties
    string strStructVar = valRef1.vr_strVar;
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
        
        string strVar = val2.strValue;
        
        // direct 'val1 = val1.sStruct' empties its own struct before getting a value from it
        CLdsStruct sCopy = val1.sStruct;

        if (sCopy.FindIndex(strVar) == -1) {
          LdsThrow(LEX_STRUCTVAR, "Variable '%s' does not exist in the structure at %s", strVar.c_str(), _ca->PrintPos().c_str());
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
        
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", _ca->lt_valValue.iValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }
    
    _pavalStack->Push(CLdsValueRef(val1, valRef1.vr_pvar, pvalStructAccess, valRef1.vr_strVar, strStructVar, bConstVar, valRef1.vr_bGlobal));
    return;
  }

  // array operations
  if (eType1 == EVT_ARRAY) {
    CLdsValue *pvalArrayAccess = NULL;
    
    switch (iOperation) {
      // TODO: Add multiplication operator to copy array contents multiple times
      // operators
      case LOP_ADD: {
        if (eType2 > EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot add %s to an array at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // expand the array
        int ctResize = (val1.aArray.Count() + val2.GetIndex());
        val1.aArray.Resize(ctResize);
      } break;
        
      case LOP_SUB: {
        if (eType2 > EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot subtract %s from an array at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // shrink the array
        int ctResize = (val1.aArray.Count() - val2.GetIndex());
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
          case LOP_GT:  val1 = int(val1.aArray.Count() >  val2.aArray.Count()); break;
          case LOP_GOE: val1 = int(val1.aArray.Count() >= val2.aArray.Count()); break;
          case LOP_LT:  val1 = int(val1.aArray.Count() <  val2.aArray.Count()); break;
          case LOP_LOE: val1 = int(val1.aArray.Count() <= val2.aArray.Count()); break;
          case LOP_EQ:  val1 = int(val1 == val2); break;
          case LOP_NEQ: val1 = int(val1 != val2); break;
        }
        break;
      
      // accessor
      case LOP_ACCESS: {
        if (_ca->lt_iArg >= 1) {
          LdsThrow(LEX_BINARY, "Cannot use structure accessor on the array at %s", _ca->PrintPos().c_str());
        }
        
        if (eType2 > EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot use %s as an array accessor at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        // direct 'val1 = val1.aArray' empties its own array before getting a value from it
        CLdsArray aCopy = val1.aArray;
        
        int iArrayIndex = val2.GetIndex();
        int iSize = aCopy.Count();
    
        // out of bounds
        if (iSize <= 0) {
          LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty array at %s", _ca->PrintPos().c_str());

        } else if (iArrayIndex < 0 || iArrayIndex >= iSize) {
          LdsThrow(LEX_ARRAYOUT, "Array index '%d' is out of bounds [0, %d] at %s", iArrayIndex, iSize-1, _ca->PrintPos().c_str());
        }
        
        val1 = aCopy[iArrayIndex];
        
        // get pointer to the value within the array
        pvalArrayAccess = valRef1.GetValue(iArrayIndex);

        // add reference index
        valRef1.AddIndex(iArrayIndex);
      } break;
      
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", _ca->lt_valValue.iValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }
    
    _pavalStack->Push(CLdsValueRef(val1, valRef1.vr_pvar, pvalArrayAccess, valRef1.vr_strVar, valRef1.vr_strRef, valRef1.vr_bConst, valRef1.vr_bGlobal));
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
        int ctSub = val2.GetIndex();

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

        for (int iCopy = 0; iCopy < val2.GetIndex(); iCopy++) {
          strMul += val1.strValue;
        }

        val1 = strMul;
      } break;

      // compare strings
      case LOP_EQ: {
        bool bResult = false;

        // compare length
        if (!bStr1 && bStr2) {
          bResult = (val1.GetIndex() == val2.strValue.length());
        } else if (bStr1 && !bStr2) {
          bResult = (val2.GetIndex() == val1.strValue.length());

        // compare string contents
        } else {
          bResult = (val1 == val2);
        }

        val1 = bResult;
      } break;

      case LOP_NEQ: {
        bool bResult = false;

        // compare length
        if (!bStr1 && bStr2) {
          bResult = (val1.GetIndex() != val2.strValue.length());
        } else if (bStr1 && !bStr2) {
          bResult = (val2.GetIndex() != val1.strValue.length());

        // compare string contents
        } else {
          bResult = (val1 != val2);
        }

        val1 = bResult;
      } break;

      // accessor
      case LOP_ACCESS: {
        if (_ca->lt_iArg >= 1) {
          // TODO: Make it search for a certain substring and return amount of them in a string
          //       Example: "string".str - returns 1; "search for the sea"["sea"] - returns 2
          LdsThrow(LEX_BINARY, "Cannot use structure accessor on a string at %s", _ca->PrintPos().c_str());
        }

        if (!bStr1) {
          LdsThrow(LEX_BINARY, "Cannot apply %s accessor to %s at %s", strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
        }

        if (eType2 > EVT_FLOAT) {
          LdsThrow(LEX_BINARY, "Cannot use %s as an string accessor at %s", strType2.c_str(), _ca->PrintPos().c_str());
        }
        
        string strCopy = val1.strValue;
        
        int iCharIndex = val2.GetIndex();
        int iLength = strCopy.length();
    
        // out of bounds
        if (iLength <= 0) {
          LdsThrow(LEX_ARRAYEMPTY, "Cannot index an empty string at %s", _ca->PrintPos().c_str());

        } else if (iCharIndex < 0 || iCharIndex >= iLength) {
          LdsThrow(LEX_ARRAYOUT, "Character index '%d' is out of bounds [0, %d] at %s", iCharIndex, iLength-1, _ca->PrintPos().c_str());
        }
        
        // get one character from the string
        val1 = LdsPrintF("%c", strCopy.c_str()[iCharIndex]);
      } break;

      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d to %s and %s at %s", _ca->lt_valValue.iValue, strType1.c_str(), strType2.c_str(), _ca->PrintPos().c_str());
    }

  } else {
    // get numbers
    int iNum1 = val1.GetIndex();
    int iNum2 = val2.GetIndex();
    float fNum1 = val1.GetNumber();
    float fNum2 = val2.GetNumber();

    switch (iOperation) {
      // operators
      case LOP_ADD: val1 = (fNum1 + fNum2); break;
      case LOP_SUB: val1 = (fNum1 - fNum2); break;
      case LOP_MUL: val1 = (fNum1 * fNum2); break;
      case LOP_DIV: val1 = (fNum1 / fNum2); break;
    
      case LOP_FMOD:
        if (fNum2 != 0.0f) {
          val1 = fmodf(fNum1, fNum2);
        } else {
          val1 = 0.0f;
        }
        break;
      
      case LOP_IDIV:
        if (iNum2 != 0) {
          val1 = (iNum1 / iNum2);
        } else {
          val1 = 0;
        }
        break;
        
      // bitwise operators
      case LOP_SH_L:  val1 = (iNum1 << iNum2); break;
      case LOP_SH_R:  val1 = (iNum1 >> iNum2); break;
      case LOP_B_AND: val1 = (iNum1 &  iNum2); break;
      case LOP_B_XOR: val1 = (iNum1 ^  iNum2); break;
      case LOP_B_OR:  val1 = (iNum1 |  iNum2); break;

      // conditional operators
      case LOP_AND: val1 = (iNum1 > 0) && (iNum2 > 0); break;
      case LOP_OR:  val1 = (iNum1 > 0) || (iNum2 > 0); break;
      case LOP_XOR: val1 = ((iNum1 >= 0) ^ (iNum2 > 0)) > 0; break;

      case LOP_GT:  val1 = (fNum1 >  fNum2); break;
      case LOP_GOE: val1 = (fNum1 >= fNum2); break;
      case LOP_LT:  val1 = (fNum1 <  fNum2); break;
      case LOP_LOE: val1 = (fNum1 <= fNum2); break;
      case LOP_EQ:  val1 = (fNum1 == fNum2); break;
      case LOP_NEQ: val1 = (fNum1 != fNum2); break;
      
      default: LdsThrow(LEX_BINARY, "Cannot apply operator %d at %s", _ca->lt_valValue.iValue, _ca->PrintPos().c_str());
    }
  }
  
  _pavalStack->Push(CLdsValueRef(val1));
};

// Get variable value
void Exec_Get(void) {
  SLdsVar *pvar = NULL;
  string strName = _ca->lt_valValue.strValue;
        
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", _ca->lt_valValue.GetString(), _ca->PrintPos().c_str());
  }
  
  CLdsValue *pvalRef = &pvar->var_valValue;
  
  _pavalStack->Push(CLdsValueRef(*pvalRef, pvar, NULL, strName, strName, false, true));
};

// Set variable value
void Exec_Set(void) {
  SLdsVar *pvar = NULL;
  string strName = _ca->lt_valValue.strValue;
  
  // try to get the value
  if (!_pldsCurrent->LdsVariableValue(strName, pvar)) {
    LdsThrow(LEX_VARIABLE, "Variable '%s' is invalid at %s", _ca->lt_valValue.GetString(), _ca->PrintPos().c_str());
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
  
  string strName = _ca->lt_valValue.strValue;

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
    LdsThrow(LEX_CONST, "Cannot reassign constant variable '%s' at %s", _ca->lt_valValue.GetString(), _ca->PrintPos().c_str());
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
  if (valRef.vr_bConst) {
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
  CLdsValueRef valValue = _pldsCurrent->CallFunction(_ca->lt_valValue.strValue, avalArgs);
  
  _pavalStack->Push(valValue);
};
