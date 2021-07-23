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

// Don't import the library for this example
#define LDS_EXPORT

// Lilac Dragon Script
#include "LilacDragonScript.h"

// Script engine
static CLdsScriptEngine _ldsEngine;

#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

// Running tests of all scripts
static bool _bAllScriptsTest = false;

// Error output function
void ErrorOutput(const char *strError) {
  printf("[LDS ERROR]: %s", strError);
};

// Random number function
LDS_FUNC(LDS_Random) {
  return rand();
};

// Console printing function
LDS_FUNC(LDS_ConsolePrint) {
  // don't output anything on tests
  if (_bAllScriptsTest) {
    return 0;
  }

  CLdsValue valPrint = LDS_NEXT_ARG;
  return printf("%s", valPrint->Print().c_str());
};

// Suspend execution for some time
LDS_FUNC(LDS_Sleep) {
  int iMilliseconds = int(LDS_NEXT_NUM * 1000);

  // wait for very little while doing all tests
  if (_bAllScriptsTest) {
    iMilliseconds = 10;
  }
  
  #ifndef WIN32
  LdsError("This method is only available on Windows systems!");
  #else
  Sleep(iMilliseconds);
  #endif

  return 0;
};

// Return an array or an object with some data
LDS_FUNC(LDS_Data) {
  int iObject = LDS_NEXT_INT;

  CLdsArrayType aData;
  aData.Add(0xFF);
  aData.Add(0x7F);

  if (iObject != 0) {
    // return object
    CLdsVars aFields;
    aFields.Add() = SLdsVar("info", string("Two bytes"), true);
    aFields.Add() = SLdsVar("data", aData, true);

    return CLdsObjectType(-1, aFields, true);
  }

  // return array
  CLdsArrayType valArray;
  valArray.Add(string("Two bytes"));
  valArray.Add(aData);

  return valArray;
};

// Initial LDS setup
void SetupLDS(void) {
  // hook the error output function
  _ldsEngine.LdsOutputFunctions(NULL, ErrorOutput);

  // cache scripts
  _ldsEngine._bUseScriptCaching = true;

  // custom functions
  CLdsFuncMap mapFunc;
  mapFunc["Random"] = SLdsFunc(0, &LDS_Random);
  mapFunc["Out"] = SLdsFunc(1, &LDS_ConsolePrint);
  mapFunc["Sleep"] = SLdsFunc(1, &LDS_Sleep);
  mapFunc["GetData"] = SLdsFunc(1, &LDS_Data);

  _ldsEngine.SetCustomFunctions(mapFunc);

  // custom variables (values are preserved between runs)
  CLdsVars aVars;
  aVars.Add() = SLdsVar("MAX_COUNT", 10, true);
  aVars.Add() = SLdsVar("strHello", string("Hello, world!"));
  
  _ldsEngine.SetCustomVariables(aVars);
};



// Test one script
static bool RunScript(string strFile, const bool &bInfo)
{
  strFile = string("TestScripts\\") + strFile;

  if (bInfo) {
    printf("[LDS]: Running \"%s\"...\n", strFile.c_str());
  }

  // load script from the file
  string strScript = "";

  // can also use "_ldsEngine._pLdsLoadScript" for the engine-specific function
  if (!LdsLoadScriptFile(strFile.c_str(), strScript)) {
    ErrorOutput("Couldn't load the script file\n\n");
    return false;
  }

  // list of compiled actions
  CActionList acaActions;

  // compile the script
  ELdsError eResult = _ldsEngine.LdsCompileScript(strScript, acaActions);
  
  // didn't compile OK
  if (eResult != LER_OK) {
    printf("\n");
    return false;
  }

  // display action count
  if (bInfo) {
    printf("[LDS]: Compiled %d actions\n", acaActions.Count());

    if (!_bAllScriptsTest) {
      printf("--------------------------------\n");
    }
  }
  
  // execute script in quick run mode
  CLdsQuickRun qrScript(_ldsEngine, acaActions);
  
  if (qrScript.GetStatus() != ETS_FINISHED) {
    // execution has failed
    printf("\n");
    return false;
  }
  
  // print out the result
  if (bInfo) {
    if (!_bAllScriptsTest) {
      printf("\n--------------------------------\n");
    }

    // action count
    int ctActions = qrScript.qr_psthThread->sth_ctActions;
    
    printf("[LDS]: Executed %d actions\n", ctActions);
    printf("[RESULT]: %s\n\n", qrScript.GetResult()->Print().c_str());
  }

  // ran successfully
  return true;
};

// Test all scripts
static void RunAllScripts(void) {
  #ifndef WIN32
  printf("Only supported on Windows systems\n");
  #else

  _bAllScriptsTest = true;
  printf("\n");

  WIN32_FIND_DATA fndData;
  HANDLE hFind = FindFirstFile("TestScripts\\*.lds", &fndData);

  // no matching files
  if (hFind == INVALID_HANDLE_VALUE) {
    printf("No scripts has been found\n\n");
    return;
  }

  int ctScripts = 0;
  int ctPassed = 0;
    
  // as long as the file is found
  while (hFind != INVALID_HANDLE_VALUE) {
    // run this script
    string strFile = fndData.cFileName;

    if (RunScript(strFile, true)) {
      // count passed scripts
      ctPassed++;
    }

    // count the script
    ctScripts++;

    // find next script
    if (!FindNextFile(hFind, &fndData)) {
      // not found
      FindClose(hFind);
      break;
    }
  }

  printf("[LDS]: Ran %d/%d scripts successfully\n\n", ctPassed, ctScripts);
  #endif
};

// Entry point
int main() {
  SetupLDS();

  // forever
  while (true) {
    _bAllScriptsTest = false;

    // display help
    RunScript("UsageExampleHelp.lds", false);

    // user input
    string strInput;
    std::getline(std::cin, strInput);

    // retrive action number
    char *chr;
    int iAction = strtol(strInput.c_str(), &chr, 10);

    // not a number
    if (*chr) {
      printf("- Invalid input\n\n");
      continue;
    }

    switch (iAction) {
      // test all scripts
      case 1: RunAllScripts(); break;

      // test one script
      case 2:
        printf("Enter script name: ");
        std::getline(std::cin, strInput);

        printf("\n");

        strInput += ".lds";
        RunScript(strInput, true);
        break;

      // view cached scripts
      case 3:
        if (_ldsEngine._mapScriptCache.Count() <= 0) {
          printf("No scripts has been cached\n");

        } else {
          for (int iCache = 0; iCache < _ldsEngine._mapScriptCache.Count(); iCache++) {
            LdsHash iHash = _ldsEngine._mapScriptCache.GetKey(iCache);
            CActionList &aca = _ldsEngine._mapScriptCache.GetValue(iCache).acaCache;

            printf("%d - %.8X (%d actions)\n", iCache + 1, iHash, aca.Count());
          }
        }

        printf("\n");
        break;

      // quit
      case 4: return 0;

      default:
        printf("- Invalid action number\n\n");
        continue;
    }
  }

  return 0;
};
