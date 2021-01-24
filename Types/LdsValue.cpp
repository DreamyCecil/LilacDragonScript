#include "../LdsTypes.h"

// Copy constructor
CLdsValue::CLdsValue(const CLdsValue &valOther) {
  operator=(valOther);
};

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
CLdsValue &CLdsValue::Assert(const ELdsValueType &eDesired) {
  if (eType == eDesired) {
    return *this;
  }
  
  // type mismatch
  string strExpected = ::TypeName(eDesired, "a number", "a string", "an array", "a structure");
  string strGot = TypeName("a number", "a string", "an array", "a structure");
  
  LdsThrow(LER_TYPE, "Expected %s but got %s", strExpected.c_str(), strGot.c_str());

  return *this;
};

CLdsValue &CLdsValue::AssertNumber(void) {
  if (eType <= EVT_FLOAT) {
    return *this;
  }

  // type mismatch
  string strGot = TypeName("a number", "a string", "an array", "a structure");
  LdsThrow(LER_TYPE, "Expected a number but got %s", strGot.c_str());

  return *this;
};
  
// Print the value
string CLdsValue::Print(void) {
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

// Get integer value
int CLdsValue::GetIndex(void) {
  switch (eType) {
    case EVT_INDEX: return iValue;
    case EVT_FLOAT: return (int)fValue;
  }
  return 0;
};

// Get any number value
float CLdsValue::GetNumber(void) {
  switch (eType) {
    case EVT_INDEX: return (float)iValue;
    case EVT_FLOAT: return fValue;
  }
  return 0.0f;
};

// Get pure string value
const char *CLdsValue::GetString(void) {
  return strValue.c_str();
};

// Assignment
CLdsValue &CLdsValue::operator=(const CLdsValue &valOther) {
  if (this == &valOther) {
    return *this;
  }
  
  // clear other values
  Clear();
  eType = valOther.eType;

  switch (eType) {
    case EVT_INDEX: iValue = valOther.iValue; break;
    case EVT_FLOAT: fValue = valOther.fValue; break;
    case EVT_STRING: strValue = valOther.strValue; break;
    case EVT_ARRAY: aArray = valOther.aArray; break;
    case EVT_STRUCT: sStruct = valOther.sStruct; break;
  }

  return *this;
};

CLdsValue &CLdsValue::operator=(const int &i) {
  Clear();
  eType = EVT_INDEX;
  iValue = i;

  return *this;
};

CLdsValue &CLdsValue::operator=(const float &f) {
  Clear();
  eType = EVT_FLOAT;
  fValue = f;

  return *this;
};

CLdsValue &CLdsValue::operator=(const string &str) {
  Clear();
  eType = EVT_STRING;
  strValue = str;

  return *this;
};
  
// Conditions
bool CLdsValue::IsTrue(void) {
  switch (eType) {
    case EVT_INDEX:  return (iValue > 0);
    case EVT_FLOAT:  return (fValue >= 0.5f);
    case EVT_STRING: return (strValue == "");
    case EVT_ARRAY:  return (aArray.Count() > 0);
    case EVT_STRUCT: return (sStruct.iID != -1);
  }
    
  return false;
};
  
bool CLdsValue::operator==(const CLdsValue &valOther) const {
  if (eType != valOther.eType) {
    return false;
  }
    
  switch (eType) {
    case EVT_INDEX:  return (iValue == valOther.iValue);
    case EVT_FLOAT:  return (fValue == valOther.fValue);
    case EVT_STRING: return (strValue == valOther.strValue);
    case EVT_ARRAY:  return (aArray.Count() == valOther.aArray.Count());
    case EVT_STRUCT: return (sStruct.iID == valOther.sStruct.iID);
  }
    
  return false;
};
  
bool CLdsValue::operator!=(const CLdsValue &valOther) const {
  return !(operator==(valOther));
};
