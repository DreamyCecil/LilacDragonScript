// Lilac Dragon Script
#include "LdsHeader.h"

// Script engine
static CLdsScriptEngine _ldsEngine;



// Error output function
void ErrorOutput(const char *strError) {
  printf("[LDS ERROR]: %s", strError);
};

// Random number function
LdsReturn LDS_Random(CLdsValue *) {
  return rand();
};

// Output function
LdsReturn LDS_Print(CLdsValue *pArgs) {
  const char *strPrint = LDS_NEXTSTR(pArgs).c_str();
  printf("%s\n", strPrint);

  return strPrint;
};

// Initial LDS setup
void SetupLDS(void) {
  // hook the error output function
  _ldsEngine._pLdsErrorFunction = (void (*)(const char *))ErrorOutput;

  // custom functions
  CLdsFuncMap mapFunc;
  mapFunc["Random"] = SLdsFunc(0, &LDS_Random);
  mapFunc["Print"] = SLdsFunc(1, &LDS_Print);

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

  // Script that gets 10 random characters from the string and prints them out.
  // Runs it MAX_COUNT times in a loop.

  const char *strScript =
  R"( Print("Original string: " + strHello);
      Print(""); // empty line

      var ctRepeat = MAX_COUNT;
  
      while (--ctRepeat >= 0) {
        var strRandom = "";

        var ctRnd = 10;
        while (--ctRnd >= 0) {
          strRandom += strHello[Random() % 13];
        }

        Print((MAX_COUNT-ctRepeat) + ": " + strRandom);
      }

      Print(""); // empty line

      // array as a result
      strHello = "Goodbye";
      return [false, 1.5, strHello];
  )";

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
