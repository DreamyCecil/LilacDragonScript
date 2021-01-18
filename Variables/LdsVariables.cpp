#include "../LdsScriptEngine.h"

// Set default variables
void CLdsScriptEngine::SetDefaultVariables(void) {
  // set default variables
};

// Set custom variables from the map
void CLdsScriptEngine::SetCustomVariables(CLdsVarMap &mapFrom) {
  // reset the map
  _mapLdsVariables.Clear();
  
  // readd default variables
  _mapLdsVariables.CopyMap(_mapLdsDefVar);
  
  // add custom variables
  _mapLdsVariables.AddFrom(mapFrom, true);
};

// Add more variables and replace ones that already exist
void CLdsScriptEngine::AddCustomVariables(CLdsVarMap &mapFrom) {
  // add custom variables
  _mapLdsVariables.AddFrom(mapFrom, true);
};

// Get value from a number variable by name
bool CLdsScriptEngine::LdsVariableValue(const string &strVar, SLdsVar *&pvar) {
  // check the variable
  if (_mapLdsVariables.FindKeyIndex(strVar) != -1) {
    // return the variable
    pvar = &_mapLdsVariables[strVar];
    return true;
  }

  return false;
};
