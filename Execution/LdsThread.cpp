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
#include "LdsThread.h"
#include "LdsExecution.h"

extern CLdsScriptEngine *_pldsCurrent;
extern DSStack<CLdsValueRef> *_pavalStack;
extern CLdsProgram *_ppgCurrent;

extern CCompAction &SetCurrentAction(CCompAction *pcaCurrent);

// Currently active thread
extern CLdsThread *_psthCurrent = NULL;

// Current action position
extern int LDS_iActionPos = 0;

// Constructor
CLdsThread::CLdsThread(const CLdsProgram &pg, CLdsScriptEngine *plds) :
  sth_pldsEngine(plds), sth_ubFlags(0),
  sth_pgProgram(pg), sth_iPos(0), sth_ctActions(0),
  sth_eStatus(ETS_FINISHED), sth_eError(LER_OK),
  sth_pReference(NULL), sth_pPreRun(NULL), sth_pResult(NULL)
{
};

// Destructor
CLdsThread::~CLdsThread(void) {
  Clear();
};

// Clear the thread
void CLdsThread::Clear(void) {
  sth_avalStack.Clear();
  sth_aiJumpStack.Clear();
  sth_aLocals.Clear();

  sth_pgProgram.Clear();
  sth_eStatus = ETS_FINISHED;
};

// Run the thread
bool CLdsThread::Run(CLdsThread **ppsth) {
  // resume the thread
  EThreadStatus eResume = Resume();
  
  switch (eResume) {
    // return the value
    case ETS_FINISHED: {
      if (sth_pResult != NULL) {
        sth_pResult(this);
      }
    } break;
      
    // error has occured
    case ETS_ERROR: {
      string strError = sth_valResult->GetString();
      sth_pldsEngine->LdsErrorOut("%s (code: 0x%X)\n", strError.c_str(), sth_eError);
    } break;
  }
  
  // delete the thread
  if (eResume != ETS_PAUSE) {
    delete this;
    
    // empty the pointer on the outside
    if (ppsth != NULL) {
      *ppsth = NULL;
    }
  }
  
  // ran successfully
  return (eResume != ETS_ERROR);
};

// Resume the thread
EThreadStatus CLdsThread::Resume(void) {
  CActionList &aca = sth_pgProgram.acaProgram;

  // nothing to execute
  if (aca.Count() <= 0) {
    sth_valResult = 0;
    sth_eStatus = ETS_FINISHED;
    return ETS_FINISHED;
  }

  // call the pre-run function
  if (sth_pPreRun != NULL) {
    sth_pPreRun(this);
  }

  // remember previous thread
  CLdsProgram *ppgPrev = _ppgCurrent;
  CLdsScriptEngine *pldsPrev = _pldsCurrent;
  CLdsThread *psthPrev = _psthCurrent;
  DSStack<CLdsValueRef> *pavalPrev = _pavalStack;
  
  _ppgCurrent = &sth_pgProgram;
  _pldsCurrent = sth_pldsEngine;
  _psthCurrent = this;
  _pavalStack = &sth_avalStack;

  switch (sth_eStatus) {
    case ETS_ERROR:
    case ETS_FINISHED:
      return sth_eStatus;
  }

  sth_valResult = 0;
  sth_eStatus = ETS_RUNNING;
  
  int iPos = sth_iPos;
  int iLen = aca.Count();
  
  int iPausePos = 0;

  try {
    while (iPos < iLen) {
      CCompAction &ca = SetCurrentAction(&aca[iPos++]);

      // set current position within the script
      LDS_iActionPos = ca.lt_iPos;
      
      // current action
      int iType = ca.lt_eType;
      const char *strAction = _astrActionNames[iType];
      
      // print the current action
      if (IsDebug() && iType != LCA_DIR) {
        sth_pldsEngine->LdsOut("[LDS DEBUG]: (%d/%d - %s) - '%s', %d, %s\n", iPos, iLen, strAction, ca->Print().c_str(), ca.lt_iArg, ca.PrintPos().c_str());
      }
  
      switch (iType) {
        case LCA_VAL: Exec_Val(); break;
        case LCA_UN: Exec_Unary(); break;
        case LCA_BIN: Exec_Binary(); break;
        
        case LCA_SET:
          if (ca.lt_iArg) {
            Exec_SetLocal();
          } else {
            Exec_Set();
          }
          break;
          
        case LCA_GET:
          if (ca.lt_iArg) {
            Exec_GetLocal();
          } else {
            Exec_Get();
          }
          break;
          
        case LCA_SET_ACCESS: Exec_SetAccessor(); break;
      
        case LCA_CALL:
        case LCA_INLINE: {
          sth_iPos = iPos;
          
          // Inline function (local to the thread)
          if (iType == LCA_INLINE) {
            // make a list of arguments
            CLdsArray aArgs = MakeValueList(*_pavalStack, ca.lt_iArg);
            
            _psthCurrent->CallInlineFunction(ca->GetString(), aArgs);
            
            // reset position to go through the inline function
            iPos = 0;
            iLen = aca.Count();
            break;
          }
          
          // Global script function
          Exec_Call();
      
          // thread got paused or destroyed
          if (sth_eStatus != ETS_RUNNING) {
            iPausePos = ca.lt_iPos;
            throw sth_eStatus;
          }
        } break;
        
        // Add inline function to the list
        case LCA_FUNC: {
          SLdsInlineFunc &in = ca.ca_inFunc;
          string strFunc = ca->GetString();
          
          sth_mapInlineFunc.Add(strFunc) = in;
        } break;
        
        // Define a local variable
        case LCA_VAR: {
          string strName = ca->GetString();
          bool bConst = (ca.lt_iArg >= 1);
          
          // add to the list of inline locals
          if (sth_aicCalls.Count() > 0) {
            SLdsInlineCall &icCurrent = sth_aicCalls.Top();
            
            // format variable name
            strName = icCurrent.VarName(strName);
            
            // check if it already exists
            int iGlobal = sth_aLocals.FindIndex(strName);
            int iInline = icCurrent.astrLocals.FindIndex(strName);
            
            if (iGlobal == -1 && iInline == -1) {
              icCurrent.astrLocals.Add() = strName;
            }
          }
          
          sth_aLocals.Add() = SLdsVar(strName, 0, bConst);
        } break;
        
        // Apply a thread directive
        case LCA_DIR: {
          int iDirType = ca.lt_iArg;
          CLdsValue val = ca.lt_valValue;
          
          switch (iDirType) {
            // debug context level
            case THD_DEBUGCONTEXT:
              SetFlag(THF_DEBUG, val->IsTrue());
              break;
          }
        } break;
    
        // Jumping between actions
        case LCA_JUMP: {
          iPos = ca.lt_iArg;
        } break;
      
        case LCA_JUMPIF: {
          CLdsValue val = _pavalStack->Pop().vr_val;
          
          if (val->IsTrue()) {
            iPos = ca.lt_iArg;
          }
        } break;
    
        case LCA_JUMPUNLESS: {
          CLdsValue val = _pavalStack->Pop().vr_val;
          
          if (!val->IsTrue()) {
            iPos = ca.lt_iArg;
          }
        } break;
      
        case LCA_AND: {
          CLdsValue val = _pavalStack->Top().vr_val;
          
          if (val->IsTrue()) {
            _pavalStack->Pop();
          } else {
            iPos = ca.lt_iArg;
          }
        } break;
      
        case LCA_OR: {
          CLdsValue val = _pavalStack->Top().vr_val;
          
          if (val->IsTrue()) {
            iPos = ca.lt_iArg;
          } else {
            _pavalStack->Pop();
          }
        } break;
    
        // Switch block
        case LCA_SWITCH: {
          CLdsValue valCase = _pavalStack->Pop().vr_val;
          CLdsValue valDesired = _pavalStack->Top().vr_val;
      
          if (valCase == valDesired) {
            _pavalStack->Pop();
            iPos = ca.lt_iArg;
          }
        } break;
      
        // Finish execution
        case LCA_RETURN: iPos = iLen; break;

        // Discard the last entry
        case LCA_DISCARD: _pavalStack->Pop(); break;

        // Duplicate the last entry
        case LCA_DUP: {
          // get the reference first in case Push() is done before Top()
          CLdsValueRef &valTop = _pavalStack->Top();
          _pavalStack->Push() = valTop;
        } break;
      
        default: LdsThrow(LEX_ACTION, "Can't run action %s at %s", strAction, ca.PrintPos().c_str());
      }
      
      // return from an inline function
      if (iPos == iLen && sth_aicCalls.Count() > 0) {
        // return value
        CLdsValueRef valRefResult = GetResult();
        
        // restore position
        iPos = ReturnFromInline();
        iLen = aca.Count();
        
        // add result to the previous stack
        _pavalStack->Push() = valRefResult;
      }

      // count one executed action
      sth_ctActions++;
    }

    sth_eStatus = ETS_FINISHED;
    sth_valResult = GetResult().vr_val;

  // plain external error
  } catch (char *strError) {
    sth_eStatus = ETS_ERROR;
    sth_eError = LEX_THREAD;
    sth_valResult.FromString(strError);

  // LDS-specific error
  } catch (SLdsError leError) {
    sth_eStatus = ETS_ERROR;
    sth_eError = leError.le_eError;
    sth_valResult = leError.le_strMessage;
    
  } catch (EThreadStatus eStatus) {
    // something went wrong
    if (eStatus != ETS_PAUSE) {
      sth_eStatus = ETS_ERROR;
      sth_eError = LEX_THREAD;
      sth_valResult.FromString(string("The thread got destroyed at ") + LdsPrintPos(iPausePos));
    }
  }

  sth_iPos = iPos;
  
  // restore previous thread
  _ppgCurrent = ppgPrev;
  _pldsCurrent = pldsPrev;
  _psthCurrent = psthPrev;
  _pavalStack = pavalPrev;

  return sth_eStatus;
};

// Pause the thread
void CLdsThread::Pause(void) {
  if (this == NULL) {
    LdsThrow(LEX_PAUSE, "Cannot pause a thread that does not exist");
    return;
  }
  
  // thread is in quick run mode
  if (IsQuick()) {
    LdsThrow(LEX_QUICKRUN, "Cannot pause a thread that is in a quick run mode");
    return;
  }
  
  sth_eStatus = ETS_PAUSE;
};

// Get thread result
CLdsValueRef CLdsThread::GetResult(void) {
  DSStack<CLdsValueRef> *paval = &sth_avalStack;
  
  // get inline stack
  if (sth_aicCalls.Count() > 0) {
    paval = &sth_aicCalls.Top().avalStack;
  }
  
  if (paval->Count() <= 0) {
    return CLdsValueRef(0);
  }
  
  return paval->Pop();
};

// Call the inline function
void CLdsThread::CallInlineFunction(string strFunc, CLdsArray &aArgs) {
  // get the inline function
  int iInline = sth_mapInlineFunc.FindKeyIndex(strFunc);
  SLdsInlineFunc inFunc = sth_mapInlineFunc.GetValue(iInline);
  
  CLdsProgram &pgFunc = inFunc.in_pgFunc;
  CLdsInlineArgs &astrArgs = inFunc.in_astrArgs;
  
  // create an inline call
  SLdsInlineCall icCall(strFunc, sth_iPos);
  
  // create argument variables
  CLdsVars aInlineArgs;
  
  // copy argument values
  for (int iArg = 0; iArg < astrArgs.Count(); iArg++) {
    // inline variable name
    string strArg = astrArgs[iArg];
    string strInline = icCall.VarName(strArg);
    
    aInlineArgs.Add() = SLdsVar(strInline, aArgs[iArg]);
    
    // add to the list of inline locals
    if (icCall.astrLocals.FindIndex(strInline) == -1) {
      icCall.astrLocals.Add() = strInline;
    }
  }
  
  // store original program
  icCall.pgReturn = sth_pgProgram;
  
  // set new program
  sth_pgProgram = pgFunc;
  sth_iPos = 0;
  
  // add argument variables
  sth_aLocals.AddFrom(aInlineArgs, true);
  
  int iCall = sth_aicCalls.Push(icCall);
  
  // switch to inline stack
  _pavalStack = &sth_aicCalls[iCall].avalStack;
};

// Return from the inline function
int CLdsThread::ReturnFromInline(void) {
  // get the inline call
  SLdsInlineCall icCall = sth_aicCalls.Pop();
  
  // return position
  sth_iPos = icCall.iPos;
  
  // restore program
  sth_pgProgram = icCall.pgReturn;
  
  // remove inline locals
  for (int iLocal = 0; iLocal < icCall.astrLocals.Count(); iLocal++) {
    string strLocal = icCall.astrLocals[iLocal];
    sth_aLocals.Delete(strLocal);
  }
  
  // restore the stack
  _pavalStack = &sth_avalStack;
  
  if (sth_aicCalls.Count() > 0) {
    _pavalStack = &sth_aicCalls.Top().avalStack;
  }
  
  return sth_iPos;
};

// Fill a value array with values from the stack
CLdsArray MakeValueList(DSStack<CLdsValueRef> &avalStack, int ctValues) {
  // make a list of values
  CLdsArray aValues;
  aValues.New(ctValues);

  while (ctValues-- > 0) {
    aValues[ctValues] = avalStack.Pop().vr_val;
  }
  
  return aValues;
};
