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

#include "LdsThread.h"

// Class for starting threads in a quick run mode
class LDS_API CLdsQuickRun {
  public:
    CLdsThread *qr_psthThread; // thread for running the script
    EThreadStatus qr_eStatus; // thread execution status

  public:
    // Constructor
    CLdsQuickRun(CLdsScriptEngine &ldsEngine, const CActionList &acaActions,
                 CLdsVarMap &mapArgs = CLdsVarMap(), CLdsInFuncMap &mapInline = CLdsInFuncMap());

    // Destructor
    ~CLdsQuickRun(void);

    // Get thread status
    inline EThreadStatus &GetStatus(void) {
      return qr_eStatus;
    };

    // Get thread result
    inline CLdsValue &GetResult(void) {
      return qr_psthThread->sth_valResult;
    };
};
