#include "../LdsScriptEngine.h"
#include <math.h>

extern CLdsThread *_psthCurrent;

// Create a new thread
CLdsThread *CLdsScriptEngine::ThreadCreate(CActionList acaActions, CLdsVarMap &mapArgs) {
  CLdsThread *sthNew = new CLdsThread(acaActions, this);
  sthNew->sth_mapLocals.CopyMap(mapArgs);
  sthNew->sth_eStatus = ETS_RUNNING;

  return sthNew;
};

// Pause the thread
CLdsThread *CLdsScriptEngine::ThreadPause(void) {
  if (_psthCurrent == NULL) {
    LdsThrow(LEX_PAUSE, "No currently active thread to pause");
    return NULL;
  }
  
  // thread is in quick run mode
  if (_psthCurrent->sth_bQuickRun) {
    LdsThrow(LEX_QUICKRUN, "Cannot pause a thread that is in a quick run mode");
    return NULL;
  }
  
  _psthCurrent->sth_eStatus = ETS_PAUSE;
  return _psthCurrent;
};

// Execute the compiled script
CLdsValue CLdsScriptEngine::ScriptExecute(CActionList acaActions, CLdsVarMap &mapArgs, CLdsInFuncMap *pmapInline) {
  CLdsThread *psth = ThreadCreate(acaActions, mapArgs);
  psth->sth_bQuickRun = true;
  
  // copy provided inline function if there are any
  if (pmapInline != NULL && pmapInline->Count() > 0) {
    psth->sth_mapInlineFunc.AddFrom(*pmapInline, true);
  }
  
  CLdsValue valResult;

  switch (psth->Resume()) {
    case ETS_FINISHED:
      valResult = psth->sth_valResult;
      break;
    
    case ETS_ERROR:
      LdsErrorOut("%s (code: 0x%X)\n", psth->sth_valResult.GetString(), psth->sth_eError);
      break;
    
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
