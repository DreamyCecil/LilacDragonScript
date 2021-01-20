#include "../LdsTypes.h"

// Value type name
inline string TypeName(const ELdsValueType &eType, const string &strFloat, const string &strString, const string &strArray, const string &strStruct) {
  switch (eType) {
    case EVT_INDEX:
    case EVT_FLOAT: return strFloat;
    case EVT_STRING: return strString;
    case EVT_ARRAY: return strArray;
    case EVT_STRUCT: return strStruct;
  }
  return "<undefined>";
};

// Type assertion (for function arguments)
SLdsValue &SLdsValue::Assert(const ELdsValueType &eDesired) {
  // type mismatch
  if (eType != eDesired) {
    string strExpected = ::TypeName(eDesired, "a number", "a string", "an array", "a structure");
    string strGot = TypeName("a number", "a string", "an array", "a structure");
      
    LdsThrow(LER_TYPE, "Expected %s but got %s", strExpected.c_str(), strGot.c_str());
  }
    
  return *this;
};

SLdsValue &SLdsValue::AssertNumber(void) {
  // type mismatch
  if (eType > EVT_FLOAT) {
    string strGot = TypeName("a number", "a string", "an array", "a structure");
      
    LdsThrow(LER_TYPE, "Expected a number but got %s", strGot.c_str());
  }
    
  return *this;
};
  
// Print the value
string SLdsValue::Print(void) {
  switch (eType) {
    case EVT_INDEX: return LdsPrintF("%d", iValue);
    case EVT_FLOAT: return LdsPrintF("%s", LdsFloatStr(fValue).c_str());
    case EVT_STRING: return strValue;

    case EVT_ARRAY: {
      // array opening
      string strArray = "[";
      int ctArray = aArray.Count();
        
      if (ctArray > 0) {
        strArray = "[ ";
      }
        
      for (int iArray = 0; iArray < ctArray; iArray++) {
        if (iArray != 0) {
          // next entry
          strArray += ", ";
        }
          
        // print array entry
        strArray += aArray[iArray].Print();
      }
        
      // array closing
      strArray += " ]";
      return strArray;
    }
      
    case EVT_STRUCT: {
      // structure opening
      string strStruct = "{";
      int ctVars = sStruct.Count();
        
      if (ctVars > 0) {
        strStruct = "{ ";
      }
        
      for (int iVar = 0; iVar < ctVars; iVar++) {
        if (iVar != 0) {
          // next variable
          strStruct += ", ";
        }
          
        // print structure variable
        strStruct += sStruct.Print(iVar);
      }
        
      // structure closing
      strStruct += " }";
      return strStruct;
    }
  }
    
  return "<undefined type>";
};

// Get any number value
float SLdsValue::GetNumber(void) {
  switch (eType) {
    case EVT_INDEX: return iValue;
    case EVT_FLOAT: return fValue;
  }
  return 0.0f;
};

// Assignment
SLdsValue &SLdsValue::operator=(const SLdsValue &valOther) {
  if (this == &valOther) {
    return *this;
  }

  eType = valOther.eType;
  
  // clear containers
  aArray.Clear();
  sStruct.Clear();

  switch (eType) {
    case EVT_INDEX: iValue = valOther.iValue; break;
    case EVT_FLOAT: fValue = valOther.fValue; break;
    case EVT_STRING: strcpy(strValue, valOther.strValue); break;

    case EVT_ARRAY:
      fValue = 0.0f;
      aArray = valOther.aArray;
      break;

    case EVT_STRUCT:
      fValue = 0.0f;
      sStruct = valOther.sStruct;
      break;
  }

  return *this;
};

SLdsValue &SLdsValue::operator=(const int &i) {
  iValue = i;
  eType = EVT_INDEX;
  aArray.Clear();
  sStruct.Clear();

  return *this;
};

SLdsValue &SLdsValue::operator=(const float &f) {
  fValue = f;
  eType = EVT_FLOAT;
  aArray.Clear();
  sStruct.Clear();

  return *this;
};

SLdsValue &SLdsValue::operator=(const string &str) {
  strcpy(strValue, str.c_str());
  eType = EVT_STRING;
  aArray.Clear();
  sStruct.Clear();

  return *this;
};
  
// Conditions
bool SLdsValue::IsTrue(void) {
  switch (eType) {
    case EVT_INDEX:  return (iValue > 0);
    case EVT_FLOAT:  return (fValue >= 0.5f);
    case EVT_STRING: return (strcmp(strValue, "") == 0);
    case EVT_ARRAY:  return (aArray.Count() > 0);
    case EVT_STRUCT: return (sStruct.iID != -1);
  }
    
  return false;
};
  
bool SLdsValue::operator==(const SLdsValue &valOther) const {
  if (eType != valOther.eType) {
    return false;
  }
    
  switch (eType) {
    case EVT_INDEX:  return (iValue == valOther.iValue);
    case EVT_FLOAT:  return (fValue == valOther.fValue);
    case EVT_STRING: return (strcmp(strValue, valOther.strValue) == 0);
    case EVT_ARRAY:  return (aArray.Count() == valOther.aArray.Count());
    case EVT_STRUCT: return (sStruct.iID == valOther.sStruct.iID);
  }
    
  return false;
};
  
bool SLdsValue::operator!=(const SLdsValue &valOther) const {
  return !(operator==(valOther));
};
