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

#include "../Types/LdsStruct.h"

// Script value type
enum ELdsValueType {
  EVT_INDEX = 0,
  EVT_FLOAT,
  EVT_STRING,
  EVT_ARRAY,
  EVT_STRUCT,

  EVT_LAST,
};

// Unary operation error
LDS_API void LdsUnaryError(const CLdsValue &val, const CLdsToken &tkn);

// Binary operation error
LDS_API void LdsBinaryError(const CLdsValue &val1, const CLdsValue &val2, const CLdsToken &tkn);

// Script value base
class LDS_API ILdsValueBase {
  public:
    // Constructor
    ILdsValueBase(void) {};

    // Create new instance of this value
    virtual ILdsValueBase *MakeCopy(void) const = 0;

    // Get value type
    virtual ELdsValueType GetType(void) const = 0;

    // Clear the value
    virtual void Clear(void) = 0;

    // Value I/O
    virtual void Write(class CLdsScriptEngine *pEngine, void *pStream) = 0;
    virtual void Read(class CLdsScriptEngine *pEngine, void *pStream, CLdsValue &val) = 0;
  
  public:
    // Type name
    virtual string TypeName(void) const = 0;
  
    // Print the value
    virtual string Print(void) = 0;
  
    // Get integer value
    virtual int GetIndex(void) { return 0; };
    // Get float value
    virtual double GetNumber(void) { return 0.0; };
    // Get string value
    virtual string GetString(void) { return ""; };
    // Get array value
    virtual CLdsArray &GetArray(void) { return *((CLdsArray *)NULL); };
    // Get struct value
    virtual CLdsStruct &GetStruct(void) { return *((CLdsStruct *)NULL); };
    
    // Perform a unary operation
    virtual class CLdsValueRef UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn) = 0;
    // Perform a binary operation
    virtual class CLdsValueRef BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn) = 0;
  
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
    
    // Value constructor
    CLdsValue(const ILdsValueBase &val);
    
    // Simple constructors
    CLdsValue(const int &i);
    CLdsValue(const double &d);
    CLdsValue(const string &str);

    // Copy constructor
    CLdsValue(const CLdsValue &valOther);

    // Destructor
    ~CLdsValue(void);

    // Delete the value
    void DeleteValue(void);

    // Quick value access
    inline ILdsValueBase *operator->(void) const {
      return val_pBase;
    };
    inline operator ILdsValueBase*(void) const {
      return val_pBase;
    };
    inline ILdsValueBase &operator*(void) const {
      return *val_pBase; 
    };

  public:
    // Assignment
    CLdsValue &operator=(const CLdsValue &valOther);
    
    // Assignment by value
    CLdsValue &operator=(const int &i);
    CLdsValue &operator=(const double &d);
    CLdsValue &operator=(const string &str);
  
    // Type assertion (for function arguments)
    CLdsValue &Assert(const ILdsValueBase &valDesired);

    // Conditions
    bool operator==(const CLdsValue &valOther) const;
    bool operator!=(const CLdsValue &valOther) const;
};
