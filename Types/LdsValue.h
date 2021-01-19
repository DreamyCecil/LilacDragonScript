#pragma once

#include "LdsStruct.h"

// Script value type
enum ELdsValueType {
  EVT_FLOAT,
  EVT_STRING,
  EVT_ARRAY,
  EVT_STRUCT,
};

// Value type name
LDS_API inline string TypeName(const ELdsValueType &eType, const string &strFloat, const string &strString, const string &strArray, const string &strStruct);

// Script value
struct LDS_API SLdsValue {
  ELdsValueType eType; // value type

  union {
    float fValue; // float value
    char strValue[256]; // string value
  };

  CDArray<SLdsValue> aArray; // array of values
  SLdsStruct sStruct; // structure with value fields
  
  // TODO: Each structure would have a unique ID (for example - Serious Sam entity ID) and a function that would be called
  //       everytime a variable in the structure is changed and sends this variable (its position in the map) and a struct ID
  //       That would allow hooking onto certain external objects and change them on the spot.
  //       For example a line like 'entity.m_strName = "this";' would immediately change value in the 'm_strName' variable of this object in memory, not just the structure.

  // Constructors
  SLdsValue(void) : fValue(0.0f), eType(EVT_FLOAT) {};
  SLdsValue(const float &f) : fValue(f), eType(EVT_FLOAT) {};
  SLdsValue(const string &str) : eType(EVT_STRING) { strcpy(strValue, str.c_str()); };

  // Array constructor
  SLdsValue(const int &ct, SLdsValue valDef) :
    fValue(0.0f), eType(EVT_ARRAY)
  {
    aArray.New(ct);

    for (int i = 0; i < ct; i++) {
      aArray[i] = valDef;
    }
  };
  
  // Structure constructor
  SLdsValue(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic) :
    fValue(0.0f), eType(EVT_STRUCT), sStruct(iSetID)
  {
    sStruct.mapVars = map;
    sStruct.bStatic = bSetStatic;
  };

  // Conversions
  operator int() { return fValue; };
  operator float() { return fValue; };
  operator string() { return strValue; };
  
  // Type name
  inline string TypeName(const string &strFloat, const string &strString, const string &strArray, const string &strStruct) {
    return ::TypeName(eType, strFloat, strString, strArray, strStruct);
  };
  
  // Type assertion (for function arguments)
  SLdsValue &Assert(const ELdsValueType &eDesired);
  
  // Print the value
  string Print(void);

  // Assignment
  SLdsValue &operator=(const SLdsValue &valOther);
  SLdsValue &operator=(const float &f);
  SLdsValue &operator=(const string &str);
  
  // Conditions
  bool IsTrue(void);
  bool operator==(const SLdsValue &valOther) const;
  bool operator!=(const SLdsValue &valOther) const;
};

// Get value of the next function argument
#define LDS_NEXTARG(_Args) (*_Args++)
#define LDS_NEXTNUM(_Args) float(LDS_NEXTARG(_Args).Assert(EVT_FLOAT))
#define LDS_NEXTSTR(_Args) string(LDS_NEXTARG(_Args).Assert(EVT_STRING))
