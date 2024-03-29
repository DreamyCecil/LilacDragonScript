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
#include "LdsFunctions.h"
#include "LdsDefFunctions.h"
#include "LdsMath.h"

// Set default functions
void CLdsScriptEngine::SetDefaultFunctions(void) {
  // TODO: Implement Serious Sam-like event waiting blocks that execute a block of code after receiving a value.
  /* They work pretty much like switch-case blocks but only during the active waiting and take certain value types.
     'continue' works like 'resume' in Serious Sam and 'break' works like 'stop', which stops waiting and exits the block.
     There's no substitute to 'pass' because waiting can't be nested or accessed outside an inline function.
     
    Example:
    
    wait (time) {
      on (string str):
        if (str == "") {
          Print("empty string");
        }
        continue;
        
      on (number fNum): {
        iSet = fNum div 1;
        
        Print("number received");
      } break;
      
      on (array arr):
        iSet = arr[0];
        Print("array received");
        break;
        
      on (object o) : {
        var iID = GetID(o);
        Print("got object: " + iID);
        
        if (iID == 0) {
          iTemp = o.iValue;
        }
      } continue;
      
      other (val):
        Print("unknown value:" + val);
        continue;
    }
  */
  
  // set default functions
  _mapLdsDefFunc.Add("DebugOut") = SLdsFunc(1, &LDS_DebugOut);
  _mapLdsDefFunc.Add("PrintHex") = SLdsFunc(1, &LDS_PrintHex);
  _mapLdsDefFunc.Add("Hash") = SLdsFunc(1, &LDS_HashString);
  _mapLdsDefFunc.Add("Wait") = SLdsFunc(1, &LDS_Wait);
  
  // set math functions
  SetMathFunctions(_mapLdsDefFunc);

  // set math operators
  SetMathOperators(_mapLdsDefUnary);
  
  // add default functions
  _mapLdsFunctions.CopyMap(_mapLdsDefFunc);
  _mapLdsUnaryOps.CopyMap(_mapLdsDefUnary);
};

// Set custom functions from the map
void CLdsScriptEngine::SetCustomFunctions(CLdsFuncMap &mapFrom) {
  // reset the map
  _mapLdsFunctions.Clear();
  
  // readd default functions
  _mapLdsFunctions.CopyMap(_mapLdsDefFunc);
  
  // add custom functions
  _mapLdsFunctions.AddFrom(mapFrom, true);
};

// Add more functions and replace ones that already exist
void CLdsScriptEngine::AddCustomFunctions(CLdsFuncMap &mapFrom) {
  // add custom functions
  _mapLdsFunctions.AddFrom(mapFrom, true);
};

// Current function call
static CCompAction *_pcaFunctionCall = NULL;

// Call function from the action
LdsReturn CLdsScriptEngine::CallFunction(CCompAction *pcaAction, CLdsArray &aArgs)
{
  _pcaFunctionCall = pcaAction;

  // function name
  string strFunc = (*pcaAction)->GetString();

  // function is empty
  if (_mapLdsFunctions[strFunc].ef_pFunc == NULL) {
    LdsThrow(LEX_NOFUNC, "Function '%s' is NULL", strFunc.c_str());
  }

  // make an array of arguments
  int ctArgs = aArgs.Count();
  CLdsValue *pvalFuncArgs = new CLdsValue[ctArgs];

  // copy each argument
  for (int iArg = 0; iArg < ctArgs; iArg++) {
    pvalFuncArgs[iArg] = aArgs[iArg];
  }

  // call the function
  LdsReturn valValue = _mapLdsFunctions[strFunc].ef_pFunc(pvalFuncArgs);

  _pcaFunctionCall = NULL;

  delete[] pvalFuncArgs;
  return valValue;
};

// External function error
void LdsError(const char *strFormat, ...) {
  // format the error
  va_list arg;
  va_start(arg, strFormat);

  string strError = LdsVPrintF(strFormat, arg);
  va_end(arg);

  // generic error if called function hasn't been set
  if (_pcaFunctionCall == NULL) {
    LdsThrow(LEX_CALL, strError.c_str());
    return;
  }

  // function name and place
  string strFunc = (*_pcaFunctionCall)->GetString();
  string strPos = _pcaFunctionCall->PrintPos();

  // display function error
  LdsThrow(LEX_CALL, "%s() at %s: %s", strFunc.c_str(), strPos.c_str(), strError.c_str());
};
