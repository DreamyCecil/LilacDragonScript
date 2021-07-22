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

// Compiling the expression
static bool _bExpression = true;

// General compilation
ELdsError CLdsScriptEngine::LdsCompileGeneral(const string &strSource, CActionList &acaActions, const bool &bExpression) {
  // calculate hash value of the script
  LdsHash iScriptHash;

  // retrieve compiled script from the cache
  if (_bUseScriptCaching) {
    iScriptHash = GetHash(strSource);

    // check if it exists in the cache
    int iInCache = _mapScriptCache.FindKeyIndex(iScriptHash);

    if (iInCache != -1) {
      acaActions = _mapScriptCache.GetValue(iInCache).acaCache;
      return LER_OK;
    }
  }

  CActionList acaCompiled;
  _bExpression = bExpression;

  try {
    ParseScript(strSource);
    LdsBuild(bExpression);
    Compile(_bnNode, acaCompiled);

  } catch (SLdsError leError) {
    LdsErrorOut("%s (code 0x%X)\n", leError.le_strMessage.c_str(), leError.le_eError);
    return leError.le_eError;
  }

  // cache the script
  if (_bUseScriptCaching) {
    _mapScriptCache.Add(iScriptHash, SLdsCache(acaCompiled, bExpression));
  }

  // copy compiled actions
  acaActions.MoveArray(acaCompiled);
  return LER_OK;
};

// Compile the script
ELdsError CLdsScriptEngine::LdsCompileScript(const string &strScript, CActionList &acaActions) {
  return LdsCompileGeneral(strScript, acaActions, false);
};

// Compile the expression
ELdsError CLdsScriptEngine::LdsCompileExpression(const string &strExpression, CActionList &acaActions) {
  return LdsCompileGeneral(strExpression, acaActions, true);
};

// Get the variable
void CLdsScriptEngine::CompileGetter(CBuildNode &bn, CActionList &aca) {
  switch (bn.lt_eType) {
    // iVal
    case EBN_IDENTIFIER: {
      string strName = bn->GetString();
      
      // custom variables and constants
      if (_mapLdsVariables.FindKeyIndex(strName) != -1) {
        aca.Add(CCompAction(LCA_GET, bn.lt_iPos, strName, 0));
        
      // locals (if allowed)
      } else if (!_bExpression) {
        aca.Add(CCompAction(LCA_GET, bn.lt_iPos, strName, 1));
        
      } else {
        LdsThrow(LEC_NOVAR, "Variable '%s' does not exist at %s", strName.c_str(), bn.PrintPos().c_str());
      }
    } return;
      
    // aArr[value][...]
    case EBN_ACCESS:
      CompileAccessors(bn, aca, false);
      return;
      
    // ++iVal++ or ++++iVal (iVal++++ requires builder patching)
    case EBN_PREFIX:
    case EBN_POSTFIX:
      Compile(bn, aca);
      return;
  }

  LdsThrow(LEC_INVALIDEXP, "Unable to get data from the expression at %s", bn.PrintPos().c_str());
};

// Set the variable
void CLdsScriptEngine::CompileSetter(CBuildNode &bn, CActionList &aca) {
  switch (bn.lt_eType) {
    // iVal
    case EBN_IDENTIFIER: {
      string strName = bn->GetString();
      bool bLocal = (_mapLdsVariables.FindKeyIndex(strName) == -1);
      
      // custom variables
      if (!bLocal) {
        // can't change constants
        if (_mapLdsVariables[strName].var_bConst > 1) {
          LdsThrow(LEC_CONST, "Cannot change constant variable at %s", bn.PrintPos().c_str());
        }
      }
      
      aca.Add(CCompAction(LCA_SET, bn.lt_iPos, strName, bLocal));
    } return;
      
    // aArr[value][...]
    case EBN_ACCESS:
      CompileAccessors(bn, aca, true);
      return;

    // ++iVal++ or ++++iVal (iVal++++ requires builder patching)
    case EBN_PREFIX:
    case EBN_POSTFIX:
      Compile(bn, aca);
      return;
  }

  LdsThrow(LEC_INVALIDEXP, "Unable to set data to the expression at %s", bn.PrintPos().c_str());
};

// Breaks and continues
void CLdsScriptEngine::CompileBreakCont(CActionList &aca, int iStart, int iEnd, int iBreak, int iCont) {
  for (int i = iStart; i < iEnd; i++) {
    CCompAction &caAction = aca[i];
  
    if (caAction.lt_eType == LCA_JUMP) {
      switch (caAction.lt_iArg) {
        // breaks
        case -10:
          if (iBreak >= 0) {
            caAction.lt_iArg = iBreak;
          }
          break;
      
        // continues
        case -11:
          if (iCont >= 0) {
            caAction.lt_iArg = iCont;
          }
          break;
      }
    }
  }
};

// Shift jumping positions
void CLdsScriptEngine::CompileJumpShift(CActionList &aca, int iStart, int iShift) {
  for (int i = iStart; i < aca.Count(); i++) {
    CCompAction &caAction = aca[i];
  
    switch (caAction.lt_eType) {
      // shift all of the jumping actions
      case LCA_JUMP: case LCA_JUMPIF: case LCA_JUMPUNLESS:
      case LCA_AND: case LCA_OR:
        caAction.lt_iArg += iShift;
        break;
    }
  }
};

// Accessors
void CLdsScriptEngine::CompileAccessors(CBuildNode &bn, CActionList &aca, bool bSet) {
  // compile the initial value
  if (bn.bn_abnNodes[0]->lt_eType != EBN_DISCARD_ACT) {
    Compile(*bn.bn_abnNodes[0], aca);
  }
  
  // compile every accessor
  CBuildNode *pbnCurrentAccess = &bn;

  while (pbnCurrentAccess != NULL) {
    // add accessor value (index or variable name)
    CBuildNode *pbnAccessVal = pbnCurrentAccess->bn_abnNodes[1];
    Compile(*pbnAccessVal, aca);

    // add access action
    aca.Add(CCompAction(LCA_BIN, pbnCurrentAccess->lt_iPos, LOP_ACCESS, bn.lt_iArg));

    // no more accessors
    if (pbnCurrentAccess->bn_abnNodes.Count() <= 2) {
      break;
    }

    CBuildNode *pbnAccess = pbnCurrentAccess->bn_abnNodes[2];
  
    // if it's an accessor, compile it
    if (pbnAccess->lt_eType == EBN_ACCESS) {
      pbnCurrentAccess = pbnAccess;
    
    // not an accessor
    } else {
      pbnCurrentAccess = NULL;
    }
  }

  if (bSet) {
    aca.Add(CCompAction(LCA_SET_ACCESS, bn.lt_iPos, -1, -1));
  }
};

// Compile nodes recursively
void CLdsScriptEngine::Compile(CBuildNode &bn, CActionList &aca) {
  switch (bn.lt_eType) {
    // values
    case EBN_RAW_VAL: aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, -1)); break;
    case EBN_IDENTIFIER: CompileGetter(bn, aca); break;

    // arrays
    case EBN_ARRAY_VAL: {
      int ctValues = bn.lt_iArg;

      for (int iVal = 0; iVal < ctValues; iVal++) {
        Compile(*bn.bn_abnNodes[iVal], aca);
      }

      // value signifies if it's an array or not
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, 0, ctValues));
    } break;
    
    // structures
    case EBN_STRUCT_VAL: {
      int ctVars = bn.lt_iArg;

      for (int iVar = 0; iVar < ctVars; iVar++) {
        Compile(*bn.bn_abnNodes[iVar], aca);
      }

      // value signifies if it's a static structure or not
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, ctVars));
    } break;

    // unary operation value
    case EBN_UNARY_OP:
      Compile(*bn.bn_abnNodes[0], aca);
      aca.Add(CCompAction(LCA_UN, bn.lt_iPos, bn.lt_valValue, -1));
      break;

    // binary operation values
    case EBN_BINARY_OP:
      switch (bn->GetIndex()) {
        case LOP_AND: {
          Compile(*bn.bn_abnNodes[0], aca);
        
          CCompAction caJump = CCompAction(LCA_AND, bn.lt_iPos, -1, -1);
          int iJump = aca.Add(caJump);
        
          Compile(*bn.bn_abnNodes[1], aca);
        
          aca[iJump].lt_iArg = aca.Count();
        } break;

        case LOP_OR: {
          Compile(*bn.bn_abnNodes[0], aca);
        
          CCompAction caJump = CCompAction(LCA_OR, bn.lt_iPos, -1, -1);
          int iJump = aca.Add(caJump);
        
          Compile(*bn.bn_abnNodes[1], aca);
        
          aca[iJump].lt_iArg = aca.Count();
        } break;

        default:
          Compile(*bn.bn_abnNodes[0], aca);
          Compile(*bn.bn_abnNodes[1], aca);
          aca.Add(CCompAction(LCA_BIN, bn.lt_iPos, bn.lt_valValue, -1));
      }
      break;

    // function arguments
    case EBN_CALL_ACT: {
      // verify argument count
      int ctArgs = bn.lt_iArg;
      string strFunc = bn->GetString();

      ELdsAction eAction = LCA_CALL;
      
      // find the inline function
      if (_mapInlineFunc.FindKeyIndex(strFunc) != -1) {
        int ctFuncArgs = _mapInlineFunc[strFunc].Count();
        eAction = LCA_INLINE;
        
        if (ctArgs != ctFuncArgs) {
          LdsThrow(LEC_FUNCARG, "Inline function '%s' takes %d argument(s) but got %d at %s", strFunc.c_str(), ctFuncArgs, ctArgs, bn.PrintPos().c_str());
        }
        
      // find the function
      } else if (_mapLdsFunctions.FindKeyIndex(strFunc) != -1) {
        int ctFuncArgs = _mapLdsFunctions[strFunc].ef_iArgs;
        eAction = LCA_CALL;
        
        if (ctArgs != ctFuncArgs) {
          LdsThrow(LEC_FUNCARG, "Function '%s' takes %d argument(s) but got %d at %s", strFunc.c_str(), ctFuncArgs, ctArgs, bn.PrintPos().c_str());
        }
        
      } else {
        LdsThrow(LEC_NOFUNC, "Unknown function '%s' at %s", strFunc.c_str(), bn.PrintPos().c_str());
      }

      for (int iArg = 0; iArg < ctArgs; iArg++) {
        Compile(*bn.bn_abnNodes[iArg], aca);
      }
      aca.Add(CCompAction(eAction, bn.lt_iPos, bn.lt_valValue, ctArgs));
    } break;
    
    // inline function
    case EBN_FUNC_DEF: {
      // function name
      string strFunc = bn->GetString();
      
      // compile the function
      CActionList acaFunc;
      Compile(*bn.bn_abnNodes[0], acaFunc);
      
      // define inline function
      CCompAction caInline = CCompAction(LCA_FUNC, bn.lt_iPos, strFunc, -1);
      caInline.ca_inFunc = SLdsInlineFunc(_mapInlineFunc[strFunc], acaFunc);
      
      // put function definitions before everything else
      aca.Insert(0, caInline);
    
      // shift jumping
      CompileJumpShift(aca, 1, 1);
    } break;
    
    // variable definition
    case EBN_VAR_DEF: {
      // variable name
      string strVar = bn->GetString();
      
      // global variable redefinition
      if (_mapLdsVariables.FindKeyIndex(strVar) != -1) {
        LdsThrow(LEC_VARDEF, "Variable '%s' redefinition at %s", strVar.c_str(), bn.PrintPos().c_str());
      }
      
      // add new local variable
      aca.Add(CCompAction(LCA_VAR, bn.lt_iPos, bn.lt_valValue, bn.lt_iArg));
    } break;
    
    // structure variable definition
    case EBN_SVAR_DEF: {
      // variable value
      Compile(*bn.bn_abnNodes[0], aca);
      
      // constant variable
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_iArg, -1));
      
      // variable name
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, -1));
    } break;
    
    // thread directive
    case EBN_DIR: {
      aca.Add(CCompAction(LCA_DIR, bn.lt_iPos, bn.lt_valValue, bn.lt_iArg));
    } break;

    // block of expressions 
    case EBN_BLOCK: {
      int ctExp = bn.lt_iArg;
    
      // compile each statement
      for (int iExp = 0; iExp < ctExp; iExp++) {
        Compile(*bn.bn_abnNodes[iExp], aca);
      }
    } break;
    
    // return a value if possible
    case EBN_RETURN_ACT:
      if (bn.lt_iArg > 0) {
        Compile(*bn.bn_abnNodes[0], aca);
      }
      aca.Add(CCompAction(LCA_RETURN, bn.lt_iPos, -1, -1));
      break;
    
    // discard last value
    case EBN_DISCARD_ACT:
      Compile(*bn.bn_abnNodes[0], aca);
      aca.Add(CCompAction(LCA_DISCARD, bn.lt_iPos, -1, -1));
      break;
    
    // jump through the 'if' block unless the condition is true
    case EBN_IF_THEN: {
      Compile(*bn.bn_abnNodes[0], aca);
        
      CCompAction caJump = CCompAction(LCA_JUMPUNLESS, bn.lt_iPos, -1, -1);
      int iJump = aca.Add(caJump);
    
      Compile(*bn.bn_abnNodes[1], aca);
    
      aca[iJump].lt_iArg = aca.Count();
    } break;
  
    // jump through the 'if' block unless the condition is true, then jump through 'else'
    case EBN_IF_THEN_ELSE: {
      Compile(*bn.bn_abnNodes[0], aca);
        
      CCompAction caJumpElse = CCompAction(LCA_JUMPUNLESS, bn.lt_iPos, -1, -1);
      int iJumpElse = aca.Add(caJumpElse);
    
      Compile(*bn.bn_abnNodes[1], aca);
    
      CCompAction caJumpThen = CCompAction(LCA_JUMP, bn.lt_iPos, -1, -1);
      int iJumpThen = aca.Add(caJumpThen);

      aca[iJumpElse].lt_iArg = aca.Count();
    
      Compile(*bn.bn_abnNodes[2], aca);
    
      aca[iJumpThen].lt_iArg = aca.Count();
    } break;
    
    // switch block
    case EBN_SWITCH: {
      CDynamicNodeList &abnCases = bn.bn_abnNodes[1]->bn_abnNodes;
      CDynamicNodeList &abnActions = bn.bn_abnNodes[2]->bn_abnNodes;
      
      // amount of cases
      int ctCases = bn.bn_abnNodes[1]->lt_iArg;

      // jumps for each case
      DSArray<int> aiArgJumps;
      aiArgJumps.New(ctCases);
    
      // header value
      Compile(*bn.bn_abnNodes[0], aca);
    
      // compile case value, remember jump position for this case's actions
      for (int iCase = 0; iCase < ctCases; iCase++) {
        Compile(*abnCases[iCase], aca);
      
        CCompAction caJump = CCompAction(LCA_SWITCH, bn.lt_iPos, -1, -1);
        aiArgJumps[iCase] = aca.Add(caJump);
      }
      
      // discard the header value
      aca.Add(CCompAction(LCA_DISCARD, bn.lt_iPos, -1, -1));
    
      // remember jump position for default actions (after all cases)
      CCompAction caDefJump = CCompAction(LCA_JUMP, bn.lt_iPos, -1, -1);
      int iDefJump = aca.Add(caDefJump);
    
      int iStartPos = aca.Count();
    
      // go through cases again
      for (int iCaseActions = 0; iCaseActions < ctCases; iCaseActions++) {
        // set jump position of that case to the actions here
        int iJump = aiArgJumps[iCaseActions];
        aca[iJump].lt_iArg = aca.Count();
      
        // compile case actions
        Compile(*abnActions[iCaseActions], aca);
      }
    
      aca[iDefJump].lt_iArg = aca.Count();
    
      // get default actions
      CBuildNode *bnDefault = NULL;

      // check if default actions exist
      if (bn.bn_abnNodes.Count() > 3) {
        bnDefault = bn.bn_abnNodes[3];
      }

      // compile default actions
      if (bnDefault != NULL) {
        Compile(*bnDefault, aca);
      }
      
      int iBreakPos = aca.Count();
      CompileBreakCont(aca, iStartPos, iBreakPos, iBreakPos, -1);
    } break;
    
    case EBN_ASSIGN_OP: {
      // get the value
      if (bn->GetIndex() == LOP_SET) {
        Compile(*bn.bn_abnNodes[1], aca);
      
      // get both values then perform an operation
      } else {
        CompileGetter(*bn.bn_abnNodes[0], aca);
        Compile(*bn.bn_abnNodes[1], aca);
      
        aca.Add(CCompAction(LCA_BIN, bn.lt_iPos, bn.lt_valValue, -1));
      }
    
      // set the new value
      CompileSetter(*bn.bn_abnNodes[0], aca);
    } break;
    
    case EBN_ADJFIX: {
      // get the value
      CompileGetter(*bn.bn_abnNodes[0], aca);

      // add the value and perform the operation
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, -1));
      aca.Add(CCompAction(LCA_BIN, bn.lt_iPos, LOP_ADD, -1));
      
      // set the new value
      CompileSetter(*bn.bn_abnNodes[0], aca);
    } break;
    
    case EBN_PREFIX: {
      // get the value
      CompileGetter(*bn.bn_abnNodes[0], aca);
      
      // add the value, perform the operation and duplicate the entry
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, -1));
      aca.Add(CCompAction(LCA_BIN, bn.lt_iPos, LOP_ADD, -1));
      aca.Add(CCompAction(LCA_DUP, bn.lt_iPos, -1, -1));
      
      // set the new value
      CompileSetter(*bn.bn_abnNodes[0], aca);
    } break;
    
    case EBN_POSTFIX: {
      // get the value
      CompileGetter(*bn.bn_abnNodes[0], aca);
      
      // duplicate the entry, add the value and perform the operation
      aca.Add(CCompAction(LCA_DUP, bn.lt_iPos, -1, -1));
      aca.Add(CCompAction(LCA_VAL, bn.lt_iPos, bn.lt_valValue, -1));
      aca.Add(CCompAction(LCA_BIN, bn.lt_iPos, LOP_ADD, -1));
      
      // set the new value
      CompileSetter(*bn.bn_abnNodes[0], aca);
    } break;
    
    // this is a part of expression compilation
    case EBN_ACCESS: {
      CompileAccessors(bn, aca, false);
    } break;
    
    case EBN_WHILE_LOOP: {
      // jump through the loop unless the condition is true
      int iContPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[0], aca);
    
      CCompAction caJump = CCompAction(LCA_JUMPUNLESS, bn.lt_iPos, -1, -1);
      int iJump = aca.Add(caJump);
    
      // go through the loop and jump back
      int iStartPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[1], aca);
      aca.Add(CCompAction(LCA_JUMP, bn.lt_iPos, -1, iContPos));
    
      // break from the loop
      int iBreakPos = aca.Count();
      aca[iJump].lt_iArg = iBreakPos;
    
      CompileBreakCont(aca, iStartPos, iBreakPos, iBreakPos, iContPos);
    } break;
    
    case EBN_DO_LOOP: {
      // go through the loop
      int iStartPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[0], aca);
    
      // jump back if the condition is true
      int iContPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[1], aca);
      aca.Add(CCompAction(LCA_JUMPIF, bn.lt_iPos, -1, iStartPos));
    
      // break from the loop
      int iBreakPos = aca.Count();
      CompileBreakCont(aca, iStartPos, iBreakPos, iBreakPos, iContPos);
    } break;
    
    case EBN_FOR_LOOP: {
      Compile(*bn.bn_abnNodes[0], aca);
    
      // jump through the loop unless the condition is true
      int iLoopPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[1], aca);
    
      CCompAction caJump = CCompAction(LCA_JUMPUNLESS, bn.lt_iPos, -1, -1);
      int iJump = aca.Add(caJump);
    
      // go through the loop
      int iStartPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[3], aca);
    
      // jump back
      int iContPos = aca.Count();
    
      Compile(*bn.bn_abnNodes[2], aca);
      aca.Add(CCompAction(LCA_JUMP, bn.lt_iPos, -1, iLoopPos));
    
      // break from the loop
      int iBreakPos = aca.Count();
      aca[iJump].lt_iArg = iBreakPos;
    
      CompileBreakCont(aca, iStartPos, iBreakPos, iBreakPos, iContPos);
    } break;
    
    // break from the statement
    case EBN_BREAK_ACT:
      aca.Add(CCompAction(LCA_JUMP, bn.lt_iPos, -1, -10));
      break;
    
    // continue to the next iteration
    case EBN_CONTINUE_ACT:
      aca.Add(CCompAction(LCA_JUMP, bn.lt_iPos, -1, -11));
      break;

    default: LdsThrow(LEC_NODE, "Cannot compile node type %d at %s", bn.lt_eType, bn.PrintPos().c_str());
  }
};
