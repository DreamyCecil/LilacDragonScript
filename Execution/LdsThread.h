#pragma once

#include "../LdsTypes.h"
#include "LdsInlineCall.h"

// Thread status type
enum EThreadStatus {
  ETS_RUNNING,  // executing now
  ETS_FINISHED, // finished executing
  ETS_ERROR,    // run into some problem
  ETS_PAUSE,    // execution is paused
};

// Script thread
class LDS_API CLdsThread {
  public:
    class CLdsScriptEngine *sth_pldsEngine; // engine this thread belongs to
    bool sth_bQuickRun; // the whole thread is being executed in one run (via ScriptExecute)
    
    CActionList sth_acaActions; // compiled actions
    int sth_iPos; // current position in the thread
    
    CDStack<SLdsInlineCall> sth_aicCalls; // inline function calls
  
    CDStack<SLdsValueRef> sth_avalStack; // stack of values
    CDStack<int> sth_aiJumpStack; // stack of actions to jump to
    CLdsVarMap sth_mapLocals; // local variables to this specific thread (script)
    CLdsInFuncMap sth_mapInlineFunc; // inline functions
  
    SLdsValue sth_valResult; // value or error depending on status
    EThreadStatus sth_eStatus; // current thread status
    ELdsError sth_eError; // error code for the error status
    
    void *sth_pReference; // some reference data for the result function
    void (*sth_pResult)(CLdsThread *psth); // function call in the end of the run
    bool sth_bDebugOutput; // debug output for every action

    // Constructor & destructor
    CLdsThread(CActionList aca, CLdsScriptEngine *plds);
    ~CLdsThread(void);

    // Clear the thread
    void Clear(void);
    
    // Run the thread
    bool Run(CLdsThread **ppsth = NULL);

    // Resume the thread
    EThreadStatus Resume(void);
    
    // Get thread result
    SLdsValueRef GetResult(void);
    
    // Call the inline function
    void CallInlineFunction(string strFunc, CLdsValueList &avalArgs);
    
    // Return from the inline function
    int ReturnFromInline(void);

    // Writing and reading
    void Write(void);
    void Read(void);
};

// Fill a value list with values from the stack
CLdsValueList MakeValueList(CDStack<SLdsValueRef> &avalStack, int ctValues);

// TEMP
void ThreadOut(bool bPush);
