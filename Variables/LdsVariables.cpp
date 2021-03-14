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
