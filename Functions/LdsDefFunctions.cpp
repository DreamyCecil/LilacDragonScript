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
#include "LdsDefFunctions.h"

extern CLdsScriptEngine *_pldsCurrent;
extern CLdsThread *_psthCurrent;

// Debug output
LDS_FUNC(LDS_DebugOut) {
  CLdsValue valPrint = LDS_NEXT_ARG;
  _pldsCurrent->LdsOut("%s\n", valPrint->Print().c_str());
  
  return valPrint;
};

// Print a hexadecimal number
LDS_FUNC(LDS_PrintHex) {
  int iPrint = LDS_NEXT_INT;
  
  return LdsPrintF("%X", iPrint);
};

// Get hash value from a string
LDS_FUNC(LDS_HashString) {
  return (int)GetHash(LDS_NEXT_STR);
};

// Pause the script execution
LDS_FUNC(LDS_Wait) {
  double dWaitTime = LDS_NEXT_NUM;
  
  // current tick and wait ticks
  const double dTickRate = _pldsCurrent->_iThreadTickRate;
  LONG64 llCurrent = _pldsCurrent->_llCurrentTick;
  LONG64 llWait = LONG64(dWaitTime * dTickRate);
  
  // time when started waiting
  const double dStarted = double(llCurrent) / dTickRate;

  // try to pause the thread
  try {
    _psthCurrent->Pause();

  // couldn't pause the thread
  } catch (SLdsError leError) {
    LdsThrow(leError.le_eError, leError.le_strMessage.c_str());
    return dStarted;
  }
  
  // create a thread handler
  SLdsHandler thh(_psthCurrent, llCurrent, llCurrent + llWait);
  
  // add it to the list
  _pldsCurrent->_athhThreadHandlers.Add() = thh;
  
  // return when started
  return dStarted;
};
