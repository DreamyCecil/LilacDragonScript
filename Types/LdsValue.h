#pragma once

#include "LdsStruct.h"

// Script value type
enum ELdsValueType {
  EVT_INDEX  = 0,
  EVT_FLOAT  = 1,
  EVT_STRING = 2,
  EVT_ARRAY  = 3,
  EVT_STRUCT = 4,
};

// Value type name
LDS_API inline string TypeName(const ELdsValueType &eType, const string &strFloat, const string &strString, const string &strArray, const string &strStruct);

// Script value
struct LDS_API SLdsValue {
  ELdsValueType eType; // value type

  union {
    int iValue; // index value
    float fValue; // float value
    char *strValue; // string value
  };

  CDArray<SLdsValue> aArray; // array of values
  SLdsStruct sStruct; // structure with value fields

  // Constructors
  SLdsValue(void) : iValue(0), eType(EVT_INDEX) {};
  SLdsValue(const int &i) : iValue(i), eType(EVT_INDEX) {};
  SLdsValue(const float &f) : fValue(f), eType(EVT_FLOAT) {};

  SLdsValue(const string &str) : iValue(0), eType(EVT_STRING) {
    strValue = new char[str.length() + 1];
    strcpy(strValue, str.c_str());
  };

  // Array constructor
  SLdsValue(const int &ct, SLdsValue valDef) :
    fValue(0), eType(EVT_ARRAY)
  {
    aArray.New(ct);

    for (int i = 0; i < ct; i++) {
      aArray[i] = valDef;
    }
  };
  
  // Structure constructor
  SLdsValue(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic) :
    fValue(0), eType(EVT_STRUCT), sStruct(iSetID)
  {
    sStruct.mapVars = map;
    sStruct.bStatic = bSetStatic;
  };

  // Destructor
  ~SLdsValue(void) {
    Clear();
  };

  // Clear the values
  inline void Clear(void) {
    switch (eType) {
      case EVT_STRING: delete[] strValue; break;
      case EVT_ARRAY: aArray.Clear(); break;
      case EVT_STRUCT: sStruct.Clear(); break;
    }
  };
  
  // Type name
  inline string TypeName(const string &strFloat, const string &strString, const string &strArray, const string &strStruct) {
    return ::TypeName(eType, strFloat, strString, strArray, strStruct);
  };
  
  // Type assertion (for function arguments)
  SLdsValue &Assert(const ELdsValueType &eDesired);
  SLdsValue &AssertNumber(void);
  
  // Print the value
  string Print(void);
  
  // Get integer value
  inline int GetIndex(void);
  // Get float value
  inline float GetNumber(void);

  // Assignment
  SLdsValue &operator=(const SLdsValue &valOther);
  SLdsValue &operator=(const int &i);
  SLdsValue &operator=(const float &f);
  SLdsValue &operator=(const string &str);
  
  // Conditions
  bool IsTrue(void);
  bool operator==(const SLdsValue &valOther) const;
  bool operator!=(const SLdsValue &valOther) const;
};

// Get value of the next function argument
#define LDS_NEXTARG(_Args) (*_Args++)
#define LDS_NEXTNUM(_Args) (LDS_NEXTARG(_Args).AssertNumber().GetNumber())
#define LDS_NEXTSTR(_Args) (LDS_NEXTARG(_Args).Assert(EVT_STRING).strValue)
