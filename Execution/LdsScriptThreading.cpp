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

// Create a new thread
CLdsThread *CLdsScriptEngine::ThreadCreate(CActionList acaActions, CLdsVarMap &mapArgs) {
  CLdsThread *sthNew = new CLdsThread(acaActions, this);
  sthNew->sth_mapLocals.CopyMap(mapArgs);
  sthNew->sth_eStatus = ETS_RUNNING;

  return sthNew;
};

// Execute the compiled script
CLdsValue CLdsScriptEngine::ScriptExecute(CActionList acaActions, CLdsVarMap &mapArgs, CLdsInFuncMap *pmapInline) {
  CLdsThread *psth = ThreadCreate(acaActions, mapArgs);
  psth->SetFlag(CLdsThread::THF_QUICK, true);
  
  // copy provided inline function if there are any
  if (pmapInline != NULL && pmapInline->Count() > 0) {
    psth->sth_mapInlineFunc.AddFrom(*pmapInline, true);
  }
  
  CLdsValue valResult;

  switch (psth->Resume()) {
    case ETS_FINISHED: {
      valResult = psth->sth_valResult;
    } break;
    
    case ETS_ERROR: {
      string strError = psth->sth_valResult->GetString();
      LdsErrorOut("%s (code: 0x%X)\n", strError.c_str(), psth->sth_eError);
    } break;
    
    default:
      LdsErrorOut("Thread execution got paused inside of the 'Execute' function instead of 'ThreadResume'\n");
  }
  
  // delete the thread
  delete psth;
  
  return valResult;
};

// Thread handling
void CLdsScriptEngine::HandleThreads(const LONG64 &llCurrentTick) {
  _llCurrentTick = llCurrentTick;
  
  for (int iThread = 0; iThread < _athhThreadHandlers.Count(); iThread++) {
    SLdsHandler &thh = _athhThreadHandlers[iThread];
    CLdsThread *psth = thh.psthThread;
    
    // wait time expired
    if (llCurrentTick >= thh.llEndTime) {
      // remove the handler
      _athhThreadHandlers.Delete(iThread);
      iThread--;
      
      // run the thread again
      psth->Run();
    }
  }
};

// Get handler index of some thread if it exists
int CLdsScriptEngine::ThreadHandlerIndex(CLdsThread *psth) {
  for (int iHandler = 0; iHandler < _athhThreadHandlers.Count(); iHandler++) {
    SLdsHandler &thh = _athhThreadHandlers[iHandler];

    // found matching thread
    if (thh.psthThread == psth) {
      return iHandler;
    }
  }

  return -1;
};
