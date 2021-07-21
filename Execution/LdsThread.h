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

#pragma once

#include "../LdsTypes.h"
#include "LdsInlineCall.h"

// Current action position
extern LDS_API int LDS_iActionPos;

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

    // Thread flags
    enum ELdsThreadFlags {
      THF_QUICK = (1 << 0), // the whole thread is being executed in one run (via CLdsQuickRun)
      THF_DEBUG = (1 << 1), // debug output for every action
    };

    LdsFlags sth_ubFlags;
    
    CActionList sth_acaActions; // compiled actions
    int sth_iPos; // current position in the thread
    int sth_ctActions; // how many actions the thread has executed so far
    
    CDStack<SLdsInlineCall> sth_aicCalls; // inline function calls
  
    CDStack<CLdsValueRef> sth_avalStack; // stack of values
    CDStack<int> sth_aiJumpStack; // stack of actions to jump to
    CLdsVarMap sth_mapLocals; // local variables to this specific thread (script)
    CLdsInFuncMap sth_mapInlineFunc; // inline functions
  
    CLdsValue sth_valResult; // value or error depending on status
    EThreadStatus sth_eStatus; // current thread status
    ELdsError sth_eError; // error code for the error status

    void *sth_pReference; // some reference data for the functions
    void (*sth_pPreRun)(CLdsThread *psth); // function call before running the thread
    void (*sth_pResult)(CLdsThread *psth); // function call in the end of the run

    // Constructor & destructor
    CLdsThread(const CActionList &aca, CLdsScriptEngine *plds);
    ~CLdsThread(void);

    // Clear the thread
    void Clear(void);
    
    // Run the thread
    bool Run(CLdsThread **ppsth = NULL);

    // Resume the thread
    EThreadStatus Resume(void);
    // Pause the thread
    void Pause(void);
    
    // Get thread result
    CLdsValueRef GetResult(void);
    
    // Call the inline function
    void CallInlineFunction(string strFunc, CLdsValueList &avalArgs);
    
    // Return from the inline function
    int ReturnFromInline(void);

    // Set the flag
    inline void SetFlag(const LdsFlags ubFlag, const bool &bSet) {
      if (bSet) {
        sth_ubFlags |= ubFlag;
      } else {
        sth_ubFlags &= ~ubFlag;
      }
    };

    // Check for flags
    inline LdsFlags IsQuick(void) {
      return (sth_ubFlags & THF_QUICK);
    };
    inline LdsFlags IsDebug(void) {
      return (sth_ubFlags & THF_DEBUG);
    };

    // Writing and reading
    void Write(void);
    void Read(void);
};

// Fill a value list with values from the stack
CLdsValueList MakeValueList(CDStack<CLdsValueRef> &avalStack, int ctValues);
