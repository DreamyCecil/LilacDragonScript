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
LDS_API string TypeName(const ELdsValueType &eType,
                        const string &strNumber, const string &strString,
                        const string &strArray, const string &strStruct);

// Script value base
class LDS_API ILdsValueBase {
  public:
    // Constructor
    ILdsValueBase(void) {};

    // Destructor
    ~ILdsValueBase(void) {
      Clear();
    };

    // Get value type
    virtual ELdsValueType GetType(void) = 0;

    // Clear the value
    virtual void Clear(void) {};
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) = 0;
  
    // Print the value
    virtual string Print(void) = 0;
  
    // Get integer value
    virtual int GetIndex(void) { return 0; };
    // Get float value
    virtual double GetNumber(void) { return 0.0; };
    // Get pure string value
    virtual const char *GetString(void) { return ""; };
  
    // Conditions
    virtual bool IsTrue(void) = 0;
    virtual bool Compare(const ILdsValueBase &valOther) = 0;
};

// Script value shell
class LDS_API CLdsValue {
  public:
    ILdsValueBase *val_pBase; // actual value
    
  public:
    // Constructor
    CLdsValue(void);

    // Simple constructors
    CLdsValue(const int &i);
    CLdsValue(const double &d);
    CLdsValue(const string &str);

    // Array constructor
    CLdsValue(const int &ct, CLdsValue valDef);
  
    // Structure constructor
    CLdsValue(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic);

    // Copy constructor
    CLdsValue(const CLdsValue &valOther);

    // Destructor
    ~CLdsValue(void);

    // Delete the value
    void DeleteValue(void);

    // Get value type
    ELdsValueType GetType(void) {
      return val_pBase->GetType();
    };

    // Type conversions
    ILdsValueBase &ToVal(void) const { return *val_pBase; };
    class CLdsIntType    &ToInt(void)    const { return (class CLdsIntType &)*val_pBase; };
    class CLdsFloatType  &ToFloat(void)  const { return (class CLdsFloatType &)*val_pBase; };
    class CLdsStringType &ToString(void) const { return (class CLdsStringType &)*val_pBase; };
    class CLdsArrayType  &ToArray(void)  const { return (class CLdsArrayType &)*val_pBase; };
    class CLdsStructType &ToStruct(void) const { return (class CLdsStructType &)*val_pBase; };

  public:
    // Assignment
    CLdsValue &operator=(const CLdsValue &valOther);
    
    // Assignment by value
    CLdsValue &operator=(const int &i);
    CLdsValue &operator=(const double &d);
    CLdsValue &operator=(const string &str);
    CLdsValue &operator=(const CLdsArray &a);
    CLdsValue &operator=(const CLdsStruct &s);
  
    // Type assertion (for function arguments)
    CLdsValue &Assert(const ELdsValueType &eDesired);
    CLdsValue &AssertNumber(void);

    // Get integer value
    int GetIndex(void) const;
    // Get float value
    double GetNumber(void) const;
    // Get string value
    string &GetStringClass(void) const;
    // Get pure string value
    const char *GetString(void) const;
    // Get array value
    CLdsArray &GetArray(void) const;
    // Get struct value
    CLdsStruct &GetStruct(void) const;

    // Print the value
    inline string Print(void) {
      return val_pBase->Print();
    };

    // Conditions
    bool operator==(const CLdsValue &valOther) const;
    bool operator!=(const CLdsValue &valOther) const;
};

// LDS function arguments
#define LDS_ARGS CLdsValue *_LDS_FuncArgs

// Get value of the next function argument
#define LDS_NEXT_ARG    (*_LDS_FuncArgs++)
#define LDS_NEXT_INT    (LDS_NEXT_ARG.AssertNumber().GetIndex())
#define LDS_NEXT_NUM    (LDS_NEXT_ARG.AssertNumber().GetNumber())
#define LDS_NEXT_STR    (LDS_NEXT_ARG.Assert(EVT_STRING).GetStringClass())
#define LDS_NEXT_ARRAY  (LDS_NEXT_ARG.Assert(EVT_ARRAY).GetArray())
#define LDS_NEXT_STRUCT (LDS_NEXT_ARG.Assert(EVT_STRUCT).GetStruct())
