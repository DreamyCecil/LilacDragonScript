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

#include "LdsValue.h"

// Script integer value
class LDS_API CLdsIntType : public ILdsValueBase {
  public:
    int iValue; // index value

  public:
    // Constructor
    CLdsIntType(const int &i) : iValue(i) {};

    // Get value type
    virtual ELdsValueType GetType(void) {
      return EVT_INDEX;
    };

    // Clear the value
    virtual void Clear(void) {
      iValue = 0;
    };
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) { return strNumber; };

    // Print the value
    virtual string Print(void);
  
    // Get integer value
    virtual int GetIndex(void) { return iValue; };
    // Get float value
    virtual double GetNumber(void) { return (double)iValue; };

    // Conditions
    virtual bool IsTrue(void) {
      return (iValue != 0);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (iValue == ((CLdsIntType &)valOther).iValue);
    };
};

// Script float value
class LDS_API CLdsFloatType : public ILdsValueBase {
  public:
    double dValue; // float value

  public:
    // Constructor
    CLdsFloatType(const double &d) : dValue(d) {};

    // Get value type
    virtual ELdsValueType GetType(void) {
      return EVT_FLOAT;
    };

    // Clear the value
    virtual void Clear(void) {
      dValue = 0.0;
    };
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) { return strNumber; };

    // Print the value
    virtual string Print(void);
  
    // Get integer value
    virtual int GetIndex(void) { return (int)dValue; };
    // Get float value
    virtual double GetNumber(void) { return dValue; };

    // Conditions
    virtual bool IsTrue(void) {
      return (dValue >= 0.5);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (dValue == ((CLdsFloatType &)valOther).dValue);
    };
};

// Script string value
class LDS_API CLdsStringType : public ILdsValueBase {
  public:
    string strValue; // string value

  public:
    // Constructor
    CLdsStringType(const string &str) : strValue(str) {};

    // Get value type
    virtual ELdsValueType GetType(void) {
      return EVT_STRING;
    };

    // Clear the value
    virtual void Clear(void) {
      strValue = "";
    };
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) { return strString; };

    // Print the value
    virtual string Print(void);

    // Get pure string value
    virtual const char *GetString(void) { return strValue.c_str(); };

    // Conditions
    virtual bool IsTrue(void) {
      return (strValue == "");
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (strValue == ((CLdsStringType &)valOther).strValue);
    };
};

// Script array value
class LDS_API CLdsArrayType : public ILdsValueBase {
  public:
    CLdsArray aArray; // array of values

  public:
    // Array constructor
    CLdsArrayType(const int &ct, CLdsValue valDef) {
      aArray.New(ct);

      for (int i = 0; i < ct; i++) {
        aArray[i] = valDef;
      }
    };

    // Array copy constructor
    CLdsArrayType(const CLdsArray &a) : aArray(a) {};

    // Get value type
    virtual ELdsValueType GetType(void) {
      return EVT_ARRAY;
    };

    // Clear the value
    virtual void Clear(void) {
      aArray.Clear();
    };
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) { return strArray; };

    // Print the value
    virtual string Print(void);

    // Conditions
    virtual bool IsTrue(void) {
      return (aArray.Count() > 0);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (aArray.Count() == ((CLdsArrayType &)valOther).aArray.Count());
    };
};
  
// Script struct value
class LDS_API CLdsStructType : public ILdsValueBase {
  public:
    CLdsStruct sStruct; // structure with value fields
    
  public:
    // Structure constructor
    CLdsStructType(const int &iSetID, const CLdsVarMap &map, const bool &bSetStatic) :
      sStruct(iSetID)
    {
      sStruct.mapVars = map;
      sStruct.bStatic = bSetStatic;
    };

    // Structure copy constructor
    CLdsStructType(const CLdsStruct &s) : sStruct(s) {};

    // Get value type
    virtual ELdsValueType GetType(void) {
      return EVT_STRUCT;
    };

    // Clear the value
    virtual void Clear(void) {
      sStruct.Clear();
    };
  
  public:
    // Type name
    virtual string TypeName(const string &strNumber, const string &strString,
                            const string &strArray, const string &strStruct) { return strStruct; };

    // Print the value
    virtual string Print(void);

    // Conditions
    virtual bool IsTrue(void) {
      return (sStruct.iID != -1);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (sStruct.iID == ((CLdsStructType &)valOther).sStruct.iID);
    };
};
