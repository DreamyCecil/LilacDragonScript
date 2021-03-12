#include "LdsValueRef.h"
#include "../Variables/LdsVar.h"
#include <sstream>

// Constructor
SLdsRefIndex::SLdsRefIndex(void) :
  strIndex("0"), bIndex(true) {};

// Structure variable constructor
SLdsRefIndex::SLdsRefIndex(const string &strVar) :
  strIndex(strVar), bIndex(false) {};

// Array index constructor
SLdsRefIndex::SLdsRefIndex(const int &iIndex) : bIndex(true) {

  // write index into a stream
  std::ostringstream strStream;
  strStream << iIndex;

  // write index as a string
  strIndex = strStream.str();
};

// Get value by an array index
CLdsValue *CLdsValueRef::GetValue(const int &iIndex) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      return &vr_pvalAccess->aArray[iIndex];

    } else {
      return &vr_pvar->var_valValue.aArray[iIndex];
    }
  }

  // TODO: Make sure this is a wise decision instead of just using NULL
  // TODO: Check if something like 'val = ExternalFunc()[0][1];' worked before when it was NULL
  return &vr_val.aArray[iIndex];
};
    
// Get value by a structure variable name
CLdsValue *CLdsValueRef::GetValue(const string &strVar) {
  if (vr_pvar != NULL) {
    if (vr_pvalAccess != NULL) {
      return &vr_pvalAccess->sStruct[strVar];

    } else {
      return &vr_pvar->var_valValue.sStruct[strVar];
    }
  }
  
  // TODO: Make sure this is a wise decision instead of just using NULL
  // TODO: Check if something like 'val = ExternalFunc().structVar[1];' worked before when it was NULL
  return &vr_val.sStruct[strVar];
};
