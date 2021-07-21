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

// Create a new thread
CLdsThread *CLdsScriptEngine::ThreadCreate(const CActionList &acaActions, CLdsVarMap &mapArgs) {
  CLdsThread *sthNew = new CLdsThread(acaActions, this);
  sthNew->sth_mapLocals.CopyMap(mapArgs);
  sthNew->sth_eStatus = ETS_RUNNING;

  return sthNew;
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
