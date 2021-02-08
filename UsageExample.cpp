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

// Initial LDS setup
void SetupLDS(void) {
  // hook the error output function
  _ldsEngine.LdsOutputFunctions(NULL, ErrorOutput);

  // custom functions
  CLdsFuncMap mapFunc;
  mapFunc["Random"] = SLdsFunc(0, &LDS_Random);

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
