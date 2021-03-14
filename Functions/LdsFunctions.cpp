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

#include "../LdsScriptEngine.h"
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
        
      on (struct s) : {
        var iID = GetID(s);
        Print("got struct: " + iID);
        
        if (iID == 0) {
          iTemp = s.iValue;
        }
      } continue;
      
      other (val):
        Print("unknown value:" + val);
        continue;
    }
  */
  
  // set default functions
  _mapLdsDefFunc["Print"] = SLdsFunc(1, &LdsDebugOut);
  _mapLdsDefFunc["PrintHex"] = SLdsFunc(1, &LdsPrintHex);
  _mapLdsDefFunc["Wait"] = SLdsFunc(1, &LdsWait);
  
  // set math functions
  SetMathFunctions(_mapLdsDefFunc);
  
  // add default functions
  _mapLdsFunctions.CopyMap(_mapLdsDefFunc);
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

// Call the function
LdsReturn CLdsScriptEngine::CallFunction(string strFunc, CLdsValueList &avalArgs) {
  // function is empty
  if (_mapLdsFunctions[strFunc].ef_pFunc == NULL) {
    LdsThrow(LEX_NOFUNC, "Function '%s' is NULL", strFunc.c_str());
  }

  // make an array of arguments
  int ctArgs = avalArgs.Count();
  CLdsValue *pvalFuncArgs = new CLdsValue[ctArgs];

  // copy each argument
  for (int iArg = 0; iArg < ctArgs; iArg++) {
    pvalFuncArgs[iArg] = avalArgs[iArg];
  }

  // call the function
  LdsReturn valValue = _mapLdsFunctions[strFunc].ef_pFunc(pvalFuncArgs);

  delete[] pvalFuncArgs;
  return valValue;
};
