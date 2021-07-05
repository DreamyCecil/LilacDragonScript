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

// Lilac Dragon Script
#include "LdsHeader.h"

// Script engine
static CLdsScriptEngine _ldsEngine;



// Error output function
void ErrorOutput(const char *strError) {
  printf("[LDS ERROR]: %s", strError);
};

// Random number function
LdsReturn LDS_Random(LDS_ARGS) {
  return rand();
};

// Console printing function
LdsReturn LDS_ConsolePrint(LDS_ARGS) {
  CLdsValue valPrint = LDS_NEXT_ARG;
  return printf("%s", valPrint.Print().c_str());
};

// Initial LDS setup
void SetupLDS(void) {
  // hook the error output function
  _ldsEngine.LdsOutputFunctions(NULL, ErrorOutput);

  // custom functions
  CLdsFuncMap mapFunc;
  mapFunc["Random"] = SLdsFunc(0, &LDS_Random);
  mapFunc["Out"] = SLdsFunc(1, &LDS_ConsolePrint);

  _ldsEngine.SetCustomFunctions(mapFunc);

  // custom variables
  CLdsVarMap mapVars;
  mapVars["MAX_COUNT"] = SLdsVar(10, true);
  mapVars["strHello"] = SLdsVar(string("Hello, world!"));
  
  _ldsEngine.SetCustomVariables(mapVars);
};



// Entry point
int main() {
  SetupLDS();

  // load script from the file
  string strScript = "";

  // can also use "_ldsEngine._pLdsLoadScript" for the engine-specific function
  if (!LdsLoadScriptFile("Test.lds", strScript)) {
    printf("[LDS]: Couldn't load the script file\n");
    return 0;
  }

  printf("[LDS]: Successfully loaded the script\n");

  // list of compiled actions
  CActionList acaActions;

  // compile the script
  ELdsError eResult = _ldsEngine.LdsCompileScript(strScript, acaActions);
  
  // compiled OK
  if (eResult == LER_OK) {
    // execute script in quick run mode
    CLdsValue valResult = _ldsEngine.ScriptExecute(acaActions, CLdsVarMap());

    // print out the result
    printf("[RESULT]: %s\n", valResult.Print().c_str());
  }

  return 0;
};
