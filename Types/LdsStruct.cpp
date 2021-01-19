#include "../LdsTypes.h"

// Clear the structure
void SLdsStruct::Clear(void) {
  iID = -1;
  mapVars.Clear();
};

// Print one variable
string SLdsStruct::Print(int iVar) {
  // print value
  string strValue = mapVars.GetValue(iVar).var_valValue.Print();
  
  // var = 'val'
  string strVar = mapVars.GetKey(iVar) + " = '" + strValue + "'";
  
  return strVar;
};

// Assignment
SLdsStruct &SLdsStruct::operator=(const SLdsStruct &sOther) {
  if (this == &sOther) {
    return *this;
  }

  // copy variable fields
  Clear();
  
  iID = sOther.iID;
  mapVars = sOther.mapVars;

  return *this;
};

// Structure accessor
SLdsValue &SLdsStruct::operator[](const string &strVar) {
  return mapVars[strVar].var_valValue;
};

// Find variable index
int SLdsStruct::FindIndex(const string &strVar) {
  return mapVars.FindKeyIndex(strVar);
};
