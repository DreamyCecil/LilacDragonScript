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
class LDS_API CLdsValue {
  public:
    ELdsValueType eType; // value type

    union {
      int iValue; // index value
      float fValue; // float value
    };

    string strValue; // string value
    CLdsArray aArray; // array of values
    CLdsStruct sStruct; // structure with value fields
    
  public:
    // Constructors
    CLdsValue(void) : iValue(0), eType(EVT_INDEX) {};
    CLdsValue(const int &i) : iValue(i), eType(EVT_INDEX) {};
    CLdsValue(const float &f) : fValue(f), eType(EVT_FLOAT) {};

    CLdsValue(const string &str) : eType(EVT_STRING) {
      strValue = str;
    };

    // Array constructor
    CLdsValue(const int &ct, CLdsValue valDef) :
      iValue(0), eType(EVT_ARRAY)
    {
      aArray.New(ct);

      for (int i = 0; i < ct; i++) {
        aArray[i] = valDef;
      }
    };
  
    // Structure constructor
    CLdsValue(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic) :
      iValue(0), eType(EVT_STRUCT), sStruct(iSetID)
    {
      sStruct.mapVars = map;
      sStruct.bStatic = bSetStatic;
    };

    // Copy constructor
    CLdsValue(const CLdsValue &valOther);

    // Destructor
    ~CLdsValue(void) {
      Clear();
    };

    // Clear the values
    inline void Clear(void) {
      switch (eType) {
        case EVT_ARRAY: aArray.Clear(); break;
        case EVT_STRUCT: sStruct.Clear(); break;
      }
      iValue = 0;
    };
  
  public:
    // Type name
    inline string TypeName(const string &strFloat, const string &strString, const string &strArray, const string &strStruct) {
      return ::TypeName(eType, strFloat, strString, strArray, strStruct);
    };
  
    // Type assertion (for function arguments)
    CLdsValue &Assert(const ELdsValueType &eDesired);
    CLdsValue &AssertNumber(void);
  
    // Print the value
    string Print(void);
  
    // Get integer value
    inline int GetIndex(void);
    // Get float value
    inline float GetNumber(void);
    // Get pure string value
    inline const char *GetString(void);

    // Assignment
    CLdsValue &operator=(const CLdsValue &valOther);
    CLdsValue &operator=(const int &i);
    CLdsValue &operator=(const float &f);
    CLdsValue &operator=(const string &str);
  
    // Conditions
    bool IsTrue(void);
    bool operator==(const CLdsValue &valOther) const;
    bool operator!=(const CLdsValue &valOther) const;
};

// LDS function arguments
#define LDS_ARGS CLdsValue *_LDS_FuncArgs

// Get value of the next function argument
#define LDS_NEXT_ARG    (*_LDS_FuncArgs++)
#define LDS_NEXT_INT    (LDS_NEXT_ARG.AssertNumber().GetIndex())
#define LDS_NEXT_NUM    (LDS_NEXT_ARG.AssertNumber().GetNumber())
#define LDS_NEXT_STR    (LDS_NEXT_ARG.Assert(EVT_STRING).strValue)
#define LDS_NEXT_ARRAY  (LDS_NEXT_ARG.Assert(EVT_ARRAY).aArray)
#define LDS_NEXT_STRUCT (LDS_NEXT_ARG.Assert(EVT_STRUCT).sStruct)
