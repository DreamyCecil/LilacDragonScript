#include "../LdsTypes.h"

// Clear the structure
void CLdsStruct::Clear(void) {
  iID = -1;
  mapVars.Clear();
};

// Print one variable
string CLdsStruct::Print(int iVar) {
  // print value
  string strValue = mapVars.GetValue(iVar).var_valValue.Print();
  
  // var = 'val'
  string strVar = mapVars.GetKey(iVar) + " = '" + strValue + "'";
  
  return strVar;
};

// Assignment
CLdsStruct &CLdsStruct::operator=(const CLdsStruct &sOther) {
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
CLdsValue &CLdsStruct::operator[](const string &strVar) {
  return mapVars[strVar].var_valValue;
};

// Find variable index
int CLdsStruct::FindIndex(const string &strVar) {
  return mapVars.FindKeyIndex(strVar);
};
