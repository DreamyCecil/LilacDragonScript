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
#include "LdsQuickRun.h"

// Constructor
CLdsQuickRun::CLdsQuickRun(CLdsScriptEngine &ldsEngine, const CActionList &acaActions,
                           CLdsVars &aArgs, CLdsInFuncMap &mapInline)
{
  // create and execute the compiled script
  qr_psthThread = ldsEngine.ThreadCreate(acaActions, aArgs);
  qr_psthThread->SetFlag(CLdsThread::THF_QUICK, true);
  
  // copy provided inline function if there are any
  qr_psthThread->sth_mapInlineFunc.AddFrom(mapInline, true);

  // start the script and get its status
  qr_eStatus = qr_psthThread->Resume();

  switch (qr_eStatus) {
    // ran successfully
    case ETS_FINISHED: return;
    
    // encountered an error
    case ETS_ERROR: {
      string strError = qr_psthThread->sth_valResult->GetString();
      ldsEngine.LdsErrorOut("%s (code: 0x%X)\n", strError.c_str(), qr_psthThread->sth_eError);
    } break;
    
    // thread got paused
    default: ldsEngine.LdsErrorOut("Thread execution got paused during a quick run\n");
  }
};

// Destructor
CLdsQuickRun::~CLdsQuickRun(void) {
  // delete the thread
  delete qr_psthThread;
};
