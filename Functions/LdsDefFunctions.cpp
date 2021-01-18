#include "LdsDefFunctions.h"

extern CLdsScriptEngine *_pldsCurrent;
extern CLdsThread *_psthCurrent;

// Debug output
LdsReturn LdsDebugOut(SLdsValue *pArgs) {
  SLdsValue valPrint = LDS_NEXTARG(pArgs);
  _pldsCurrent->LdsOut("%s\n", valPrint.Print().c_str());
  
  return valPrint;
};

// Print a hexadecimal number
LdsReturn LdsPrintHex(SLdsValue *pArgs) {
  float fPrint = LDS_NEXTNUM(pArgs);
  
  return LdsPrintF("%X", (int)fPrint);
};

// Pause the script execution
LdsReturn LdsWait(SLdsValue *pArgs) {
  float fWaitTime = LDS_NEXTNUM(pArgs);
  CLdsThread *psth = _pldsCurrent->ThreadPause();
  
  // current tick and wait ticks
  const float fTickRate = _pldsCurrent->_iThreadTickRate;
  LONG64 llCurrent = _pldsCurrent->_llCurrentTick;
  LONG64 llWait = (fWaitTime * fTickRate);
  
  // time when started waiting
  const float fStarted = float(llCurrent) / fTickRate;
  
  // no thread
  if (psth == NULL) {
    LdsThrow(LEX_PAUSE, "No thread to pause");
    return fStarted;
  }
  
  // create a thread handler
  SLdsHandler thh(psth, llCurrent, llCurrent + llWait);
  
  // add it to the list
  _pldsCurrent->_athhThreadHandlers.Add(thh);
  
  // return when started
  return fStarted;
};
