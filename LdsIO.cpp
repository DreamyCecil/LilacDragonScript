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

// Compiled scripts I/O

// Write program
void CLdsScriptEngine::LdsWriteProgram(void *pStream, CActionList &acaProgram) {
  int ctActions = acaProgram.Count();
  _pLdsWrite(pStream, &ctActions, sizeof(int));

  // for each action
  for (int iAction = 0; iAction < ctActions; iAction++) {
    LdsWriteAction(pStream, acaProgram[iAction]);
  }
};

// Read program
void CLdsScriptEngine::LdsReadProgram(void *pStream, CActionList &acaProgram) {
  int ctActions = 0;
  _pLdsRead(pStream, &ctActions, sizeof(int));

  // for each action
  for (int iAction = 0; iAction < ctActions; iAction++) {
    CCompAction caAction;
    LdsReadAction(pStream, caAction);

    acaProgram.Add(caAction);
  }
};

// Write action
void CLdsScriptEngine::LdsWriteAction(void *pStream, CCompAction &caAction) {
  // write type and position
  _pLdsWrite(pStream, &caAction.lt_eType, sizeof(int));
  _pLdsWrite(pStream, &caAction.lt_iPos, sizeof(int));

  // write certain action
  switch (caAction.lt_eType) {
    // all data
    case LCA_VAL: case LCA_BIN: case LCA_VAR:
    case LCA_CALL: case LCA_INLINE:
    case LCA_SET: case LCA_GET: case LCA_DIR:
      LdsWriteValue(pStream, caAction.lt_valValue);
      _pLdsWrite(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // only value
    case LCA_UN:
      LdsWriteValue(pStream, caAction.lt_valValue);
      break;

    // inline function
    case LCA_FUNC:
      LdsWriteValue(pStream, caAction.lt_valValue);
      LdsWriteInlineFunc(pStream, caAction.ca_inFunc);
      break;

    // only argument
    case LCA_JUMP: case LCA_JUMPUNLESS: case LCA_JUMPIF:
    case LCA_AND: case LCA_OR: case LCA_SWITCH:
      _pLdsWrite(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // no data
    case LCA_SET_ACCESS:
    case LCA_RETURN: case LCA_DISCARD: case LCA_DUP:
      break;

    default: LdsThrow(LER_WRITE, "Cannot write action %s at %d!", _astrActionNames[caAction.lt_eType], _pLdsStreamTell(pStream));
  }
};

// Read action
void CLdsScriptEngine::LdsReadAction(void *pStream, CCompAction &caAction) {
  // read type and position
  _pLdsRead(pStream, &caAction.lt_eType, sizeof(int));
  _pLdsRead(pStream, &caAction.lt_iPos, sizeof(int));

  // read certain action
  switch (caAction.lt_eType) {
    // all data
    case LCA_VAL: case LCA_BIN: case LCA_VAR:
    case LCA_CALL: case LCA_INLINE:
    case LCA_SET: case LCA_GET: case LCA_DIR:
      LdsReadValue(pStream, caAction.lt_valValue);
      _pLdsRead(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // only value
    case LCA_UN:
      LdsReadValue(pStream, caAction.lt_valValue);
      break;

    // inline function
    case LCA_FUNC:
      LdsReadValue(pStream, caAction.lt_valValue);
      LdsReadInlineFunc(pStream, caAction.ca_inFunc);
      break;

    // only argument
    case LCA_JUMP: case LCA_JUMPUNLESS: case LCA_JUMPIF:
    case LCA_AND: case LCA_OR: case LCA_SWITCH:
      _pLdsRead(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // no data
    case LCA_SET_ACCESS:
    case LCA_RETURN: case LCA_DISCARD: case LCA_DUP:
      break;

    default: LdsThrow(LER_READ, "Cannot read action %s at %d!", _astrActionNames[caAction.lt_eType], _pLdsStreamTell(pStream));
  }
};

// Write inline function
void CLdsScriptEngine::LdsWriteInlineFunc(void *pStream, SLdsInlineFunc &inFunc) {
  // write argument count
  int ctArgs = inFunc.in_astrArgs.Count();
  _pLdsWrite(pStream, &ctArgs, sizeof(int));

  for (int iArg = 0; iArg < ctArgs; iArg++) {
    // write argument name
    LdsWriteString(pStream, inFunc.in_astrArgs[iArg]);
  }

  // write inline functions count
  int ctFunc = inFunc.in_mapInlineFunc.Count();
  _pLdsWrite(pStream, &ctFunc, sizeof(int));

  for (int iFunc = 0; iFunc < ctFunc; iFunc++) {
    // write function name
    LdsWriteString(pStream, inFunc.in_mapInlineFunc.GetKey(iFunc));

    // write inline function
    LdsWriteInlineFunc(pStream, inFunc.in_mapInlineFunc.GetValue(iFunc));
  }

  // write the function
  LdsWriteProgram(pStream, inFunc.in_acaFunc);
};

// Read inline function
void CLdsScriptEngine::LdsReadInlineFunc(void *pStream, SLdsInlineFunc &inFunc) {
  // read argument count
  int ctArgs = 0;
  _pLdsRead(pStream, &ctArgs, sizeof(int));

  for (int iArg = 0; iArg < ctArgs; iArg++) {
    // read argument name
    string strArg = "";
    LdsReadString(pStream, strArg);

    inFunc.in_astrArgs.Add(strArg);
  }

  // read inline functions count
  int ctFunc = 0;
  _pLdsRead(pStream, &ctFunc, sizeof(int));

  for (int iFunc = 0; iFunc < ctFunc; iFunc++) {
    // read function name
    string strFunc = "";
    LdsReadString(pStream, strFunc);

    // read inline function
    SLdsInlineFunc inInline;
    LdsReadInlineFunc(pStream, inInline);

    inFunc.in_mapInlineFunc.Add(strFunc, inInline);
  }

  // read the function
  LdsReadProgram(pStream, inFunc.in_acaFunc);
};

// Script values I/O

// Write one variable from the variable map
void CLdsScriptEngine::LdsWriteMapVar(void *pStream, CLdsVarMap &map, const int &iVar) {
  // write key
  LdsWriteString(pStream, map.GetKey(iVar));

  // write variable const type
  SLdsVar &var = map.GetValue(iVar);
  _pLdsWrite(pStream, &var.var_bConst, sizeof(char));

  // write variable value
  LdsWriteValue(pStream, var.var_valValue);
};
  
// Read one variable into the variable map
void CLdsScriptEngine::LdsReadMapVar(void *pStream, CLdsVarMap &map) {
  // read key
  string strKey = "";
  LdsReadString(pStream, strKey);

  // read variable const type
  SLdsVar var;
  _pLdsRead(pStream, &var.var_bConst, sizeof(char));

  // read variable value
  LdsReadValue(pStream, var.var_valValue);

  // add one variable
  map.Add(strKey, var);
};

// Write value
void CLdsScriptEngine::LdsWriteValue(void *pStream, CLdsValue &val) {
  int iType = val->GetType();
  _pLdsWrite(pStream, &iType, sizeof(int));

  // write value through its own method
  val->Write(this, pStream);
};

// Read value
void CLdsScriptEngine::LdsReadValue(void *pStream, CLdsValue &val) {
  int iType = -1;
  _pLdsRead(pStream, &iType, sizeof(int));

  // invalid value type
  int ctValues = _ldsValueTypes.Count();

  if (iType < 0 || iType >= ctValues) {
    LdsThrow(LER_READ, "Cannot read value type %d (%d/%d) at %d!", iType, iType + 1, ctValues, _pLdsStreamTell(pStream));
  }

  // read value through its own method
  _ldsValueTypes[iType]->Read(this, pStream, val);
};

// Write value reference
void CLdsScriptEngine::LdsWriteValueRef(void *pStream, CLdsThread &sth, CLdsValueRef &vr) {
  // write actual value
  LdsWriteValue(pStream, vr.vr_val);

  // write reference names
  LdsWriteString(pStream, vr.vr_strVar);
  LdsWriteString(pStream, vr.vr_strRef);

  // write value flags
  char eFlags = vr.vr_eFlags;
  _pLdsWrite(pStream, &eFlags, sizeof(char));

  // write variable reference index
  int iVarReference = -1; // nothing

  // some variable reference
  if (vr.vr_pvar != NULL) {
    if (vr.IsGlobal()) {
      // global index
      iVarReference = _mapLdsVariables.Index(vr.vr_pvar);

    } else {
      // local index
      iVarReference = sth.sth_mapLocals.Index(vr.vr_pvar);
    }
  }

  _pLdsWrite(pStream, &iVarReference, sizeof(int));

  // write reference index count
  int ctRef = vr.vr_ariIndices.Count();
  _pLdsWrite(pStream, &ctRef, sizeof(int));

  // write each reference index
  for (int iRef = 0; iRef < ctRef; iRef++) {
    SLdsRefIndex &ri = vr.vr_ariIndices[iRef];

    // write if it's an array index
    char bIndexRef = ri.bIndex;
    _pLdsWrite(pStream, &bIndexRef, sizeof(char));

    if (bIndexRef) {
      // write array index
      int iRefIndex = ri.GetIndex();
      _pLdsWrite(pStream, &iRefIndex, sizeof(int));

    } else {
      // write structure variable
      LdsWriteString(pStream, ri.strIndex);
    }
  }
};

// Read value reference
void CLdsScriptEngine::LdsReadValueRef(void *pStream, CLdsThread &sth, CLdsValueRef &vr) {
  // read actual value
  LdsReadValue(pStream, vr.vr_val);

  // read reference names
  LdsReadString(pStream, vr.vr_strVar);
  LdsReadString(pStream, vr.vr_strRef);

  // read value flags
  char eFlags = 0;
  _pLdsRead(pStream, &eFlags, sizeof(char));

  vr.SetFlag(eFlags, true);

  // read variable reference index
  int iVarReference = -1; // nothing
  _pLdsRead(pStream, &iVarReference, sizeof(int));

  // some variable reference
  if (iVarReference != -1) {
    if (vr.IsGlobal()) {
      // global index
      vr.vr_pvar = &_mapLdsVariables.GetValue(iVarReference);

    } else {
      // local index
      vr.vr_pvar = &sth.sth_mapLocals.GetValue(iVarReference);
    }
  }

  // read reference index count
  int ctRef = 0;
  _pLdsRead(pStream, &ctRef, sizeof(int));

  // read each reference index
  for (int iRef = 0; iRef < ctRef; iRef++) {
    // read if it's an array index
    char bIndexRef = false;
    _pLdsRead(pStream, &bIndexRef, sizeof(char));

    if (bIndexRef) {
      // read array index
      int iRefIndex = 0;
      _pLdsRead(pStream, &iRefIndex, sizeof(int));

      // add index
      vr.vr_ariIndices.Add(SLdsRefIndex(iRefIndex));

      // get next reference by an index
      vr.vr_pvalAccess = vr.GetValue(iRefIndex);

    } else {
      // read structure variable
      string strVar = "";
      LdsReadString(pStream, strVar);
      
      // add index
      vr.vr_ariIndices.Add(SLdsRefIndex(strVar));

      // get next reference by a variable name
      vr.vr_pvalAccess = vr.GetValue(strVar);
    }
  }
};

// Write string value
void CLdsScriptEngine::LdsWriteString(void *pStream, string &str) {
  const int ctLen = str.size();

  // write string length
  _pLdsWrite(pStream, &ctLen, sizeof(int));

  // write the string
  _pLdsWrite(pStream, str.c_str(), sizeof(char) * ctLen);
};

// Read string value
void CLdsScriptEngine::LdsReadString(void *pStream, string &str) {
  // read string length
  int ctLen = 0;
  _pLdsRead(pStream, &ctLen, sizeof(int));

  // make a new string
  char *strRead = new char[ctLen + 1];
  strRead[ctLen] = 0;
      
  // read the string
  _pLdsRead(pStream, strRead, sizeof(char) * ctLen);

  str = strRead;

  // discard the string
  delete[] strRead;
};

// Current scripts I/O

// Write the script engine
void CLdsScriptEngine::LdsWriteEngine(void *pStream) {
  // write script caching
  char bCaching = _bUseScriptCaching;
  _pLdsWrite(pStream, &bCaching, sizeof(char));

  // write cached scripts
  if (_bUseScriptCaching) {
    CScriptCache &mapCache = _mapScriptCache;
    int ctCached = mapCache.Count();

    // write amount of cached scripts
    _pLdsWrite(pStream, &ctCached, sizeof(int));

    for (int iCache = 0; iCache < ctCached; iCache++) {
      // write script hash
      LdsHash iHash = mapCache.GetKey(iCache);
      _pLdsWrite(pStream, &iHash, sizeof(LdsHash));

      // write compiled script
      SLdsCache &scCache = mapCache.GetValue(iCache);

      LdsWriteProgram(pStream, scCache.acaCache);

      char bExpression = scCache.bExpression;
      _pLdsWrite(pStream, &bExpression, sizeof(char));
    }
  }

  // write variable count
  int ctVars = _mapLdsVariables.Count();
  _pLdsWrite(pStream, &ctVars, sizeof(int));

  // write each variable
  for (int iVar = 0; iVar < ctVars; iVar++) {
    LdsWriteMapVar(pStream, _mapLdsVariables, iVar);
  }

  // write current tick
  _pLdsWrite(pStream, &_llCurrentTick, sizeof(LONG64));

  // write paused thread count
  int ctThreads = _athhThreadHandlers.Count();
  _pLdsWrite(pStream, &ctThreads, sizeof(int));

  // write threads
  for (int iThread = 0; iThread < ctThreads; iThread++) {
    SLdsHandler &thh = _athhThreadHandlers[iThread];

    // write wait times
    _pLdsWrite(pStream, &thh.llStartTime, sizeof(LONG64));
    _pLdsWrite(pStream, &thh.llEndTime, sizeof(LONG64));

    // write the thread
    LdsWriteThread(pStream, *thh.psthThread, false);
  }
};

// Read the script engine
void CLdsScriptEngine::LdsReadEngine(void *pStream) {
  // read script caching
  char bCaching = false;
  _pLdsRead(pStream, &bCaching, sizeof(char));

  _bUseScriptCaching = (bCaching != 0);

  // read cached scripts
  if (_bUseScriptCaching) {
    // read amount of cached scripts
    int ctCached = 0;
    _pLdsRead(pStream, &ctCached, sizeof(int));

    for (int iCache = 0; iCache < ctCached; iCache++) {
      // read script hash
      LdsHash iHash;
      _pLdsRead(pStream, &iHash, sizeof(LdsHash));

      // read compiled script
      CActionList acaCache;
      LdsReadProgram(pStream, acaCache);

      char bExpression = false;
      _pLdsRead(pStream, &bExpression, sizeof(char));

      // add to the cache list
      _mapScriptCache.Add(iHash, SLdsCache(acaCache, bExpression != 0));
    }
  }

  // read variable count
  int ctVars = 0;
  _pLdsRead(pStream, &ctVars, sizeof(int));

  // read each variable
  for (int iVar = 0; iVar < ctVars; iVar++) {
    LdsReadMapVar(pStream, _mapLdsVariables);
  }

  // read current tick
  _pLdsRead(pStream, &_llCurrentTick, sizeof(LONG64));

  // read paused thread count
  int ctThreads = 0;
  _pLdsRead(pStream, &ctThreads, sizeof(int));

  // read threads
  for (int iThread = 0; iThread < ctThreads; iThread++) {
    // read wait times
    LONG64 llStart = 0;
    LONG64 llEnd = 0;
    _pLdsRead(pStream, &llStart, sizeof(LONG64));
    _pLdsRead(pStream, &llEnd, sizeof(LONG64));

    // read the thread
    CLdsThread *psth = new CLdsThread(CActionList(), this);
    LdsReadThread(pStream, *psth, false);
      
    // add thread handler to the list
    SLdsHandler thh(psth, llStart, llEnd);
    _athhThreadHandlers.Add(thh);
  }
};

// Write thread
void CLdsScriptEngine::LdsWriteThread(void *pStream, CLdsThread &sth, bool bHandler) {
  // not this engine's thread
  if (sth.sth_pldsEngine != this) {
    LdsThrow(LER_WRITE, "Thread does not belong to the engine that's used for writing it!");
    return;
  }

  // write status and error
  char iStatus = sth.sth_eStatus;
  int iError = sth.sth_eError;
  _pLdsWrite(pStream, &iStatus, sizeof(char));
  _pLdsWrite(pStream, &iError, sizeof(int));

  // write position
  _pLdsWrite(pStream, &sth.sth_iPos, sizeof(int));

  // write the result
  LdsWriteValue(pStream, sth.sth_valResult);

  // loop indices
  int i, ct;

  // write locals count
  ct = sth.sth_mapLocals.Count();
  _pLdsWrite(pStream, &ct, sizeof(int));

  // write each local variable
  for (i = 0; i < ct; i++) {
    LdsWriteMapVar(pStream, sth.sth_mapLocals, i);
  }

  // write inline functions count
  ct = sth.sth_mapInlineFunc.Count();
  _pLdsWrite(pStream, &ct, sizeof(int));

  for (i = 0; i < ct; i++) {
    // write function name
    LdsWriteString(pStream, sth.sth_mapInlineFunc.GetKey(i));

    // write inline function
    LdsWriteInlineFunc(pStream, sth.sth_mapInlineFunc.GetValue(i));
  }

  // write stack count
  ct = sth.sth_avalStack.Count();
  _pLdsWrite(pStream, &ct, sizeof(int));

  // write each individual value from the stack
  for (i = 0; i < ct; i++) {
    LdsWriteValueRef(pStream, sth, sth.sth_avalStack[i]);
  }

  // write jump stack count
  ct = sth.sth_aiJumpStack.Count();
  _pLdsWrite(pStream, &ct, sizeof(int));

  // write each jump
  for (i = 0; i < ct; i++) {
    _pLdsWrite(pStream, &sth.sth_aiJumpStack[i], sizeof(int));
  }

  // write inline calls count
  ct = sth.sth_aicCalls.Count();
  _pLdsWrite(pStream, &ct, sizeof(int));

  // write each inline call
  for (i = 0; i < ct; i++) {
    SLdsInlineCall &icCall = sth.sth_aicCalls[i];

    // write function name and returning position
    LdsWriteString(pStream, icCall.strFunc);
    _pLdsWrite(pStream, &icCall.iPos, sizeof(int));

    int iInline, ctInline;

    // write inline locals count
    ctInline = icCall.astrLocals.Count();
    _pLdsWrite(pStream, &ctInline, sizeof(int));

    // write inline locals
    for (iInline = 0; iInline < ctInline; iInline++) {
      LdsWriteString(pStream, icCall.astrLocals[iInline]);
    }

    // write inline stack count
    ctInline = icCall.avalStack.Count();
    _pLdsWrite(pStream, &ctInline, sizeof(int));

    // write each individual value from the stack
    for (iInline = 0; iInline < ctInline; iInline++) {
      LdsWriteValueRef(pStream, sth, icCall.avalStack[iInline]);
    }

    // write actions to return
    LdsWriteProgram(pStream, icCall.acaReturn);
  }

  // write thread actions
  LdsWriteProgram(pStream, sth.sth_acaActions);

  // write handler if needed
  if (bHandler) {
    // write handler index
    int iHandler = ThreadHandlerIndex(&sth);
    _pLdsWrite(pStream, &iHandler, sizeof(int));

    if (iHandler != -1) {
      // write wait times
      SLdsHandler &thh = _athhThreadHandlers[iHandler];

      _pLdsWrite(pStream, &thh.llStartTime, sizeof(LONG64));
      _pLdsWrite(pStream, &thh.llEndTime, sizeof(LONG64));
    }
  }
};

// Read thread
void CLdsScriptEngine::LdsReadThread(void *pStream, CLdsThread &sth, bool bHandler) {
  // set as this engine's thread
  sth.sth_pldsEngine = this;

  // read status and error
  char iStatus = -1;
  int iError = -1;
  _pLdsRead(pStream, &iStatus, sizeof(char));
  _pLdsRead(pStream, &iError, sizeof(int));

  sth.sth_eStatus = (EThreadStatus)iStatus;
  sth.sth_eError = (ELdsError)iError;

  // read position
  _pLdsRead(pStream, &sth.sth_iPos, sizeof(int));

  // read the result
  LdsReadValue(pStream, sth.sth_valResult);

  // loop indices
  int i, ct;

  // read locals count
  ct = 0;
  _pLdsRead(pStream, &ct, sizeof(int));

  // read each local variable
  for (i = 0; i < ct; i++) {
    LdsReadMapVar(pStream, sth.sth_mapLocals);
  }

  // read inline functions count
  ct = 0;
  _pLdsRead(pStream, &ct, sizeof(int));

  for (i = 0; i < ct; i++) {
    // read function name
    string strFunc = "";
    LdsReadString(pStream, strFunc);

    // read inline function
    SLdsInlineFunc inInline;
    LdsReadInlineFunc(pStream, inInline);

    sth.sth_mapInlineFunc.Add(strFunc, inInline);
  }

  // read stack count
  ct = 0;
  _pLdsRead(pStream, &ct, sizeof(int));

  // read each individual value into the stack
  for (i = 0; i < ct; i++) {
    CLdsValueRef vr;
    LdsReadValueRef(pStream, sth, vr);

    sth.sth_avalStack.Push(vr);
  }

  // read jump stack count
  ct = 0;
  _pLdsRead(pStream, &ct, sizeof(int));

  // read each jump
  for (i = 0; i < ct; i++) {
    int iJump = 0;
    _pLdsRead(pStream, &iJump, sizeof(int));

    sth.sth_aiJumpStack.Add(iJump);
  }

  // read inline calls count
  ct = 0;
  _pLdsRead(pStream, &ct, sizeof(int));

  // read each inline call
  for (i = 0; i < ct; i++) {
    SLdsInlineCall icCall;

    // read function name and returning position
    LdsReadString(pStream, icCall.strFunc);
    _pLdsRead(pStream, &icCall.iPos, sizeof(int));

    int iInline, ctInline;

    // read inline locals count
    ctInline = 0;
    _pLdsRead(pStream, &ctInline, sizeof(int));

    // read inline locals
    for (iInline = 0; iInline < ctInline; iInline++) {
      string strLocal = "";
      LdsReadString(pStream, strLocal);

      icCall.astrLocals.Add(strLocal);
    }

    // read inline stack count
    ctInline = 0;
    _pLdsRead(pStream, &ctInline, sizeof(int));

    // read each individual value from the stack
    for (iInline = 0; iInline < ctInline; iInline++) {
      CLdsValueRef vr;
      LdsReadValueRef(pStream, sth, vr);

      icCall.avalStack.Push(vr);
    }

    // read actions to return
    LdsReadProgram(pStream, icCall.acaReturn);

    // add inline call
    sth.sth_aicCalls.Push(icCall);
  }

  // read thread actions
  LdsReadProgram(pStream, sth.sth_acaActions);

  // read handler if needed
  if (bHandler) {
    // read handler index
    int iHandler = -1;
    _pLdsRead(pStream, &iHandler, sizeof(int));

    if (iHandler != -1) {
      // read wait times
      LONG64 llStart = 0;
      LONG64 llEnd = 0;
      _pLdsRead(pStream, &llStart, sizeof(LONG64));
      _pLdsRead(pStream, &llEnd, sizeof(LONG64));
      
      // add thread handler to the list
      SLdsHandler thh(&sth, llStart, llEnd);
      _athhThreadHandlers.Add(thh);
    }
  }
};
