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

#include "../Types/LdsVar.h"

// Object change callback function
typedef void (*CLdsObjectCallback)(class CLdsObjectType *pvalObject, const int &iVariable);

// Script object value
class LDS_API CLdsObjectType : public ILdsValueBase {
  public:
    int iID; // unique ID
    CLdsVars aProps; // object property fields
    bool bStatic; // static object (cannot add new properties)

    CLdsObjectCallback pCallback; // callback function
    
  public:
    // Default constructor
    CLdsObjectType(void);

    // Object constructor
    CLdsObjectType(const int &iSetID, const CLdsVars &aFields, const bool &bSetStatic);

    // Assignment (illegal)
    CLdsObjectType &operator=(const CLdsObjectType &valOther);

    // Create new instance of this value
    virtual ILdsValueBase *MakeCopy(void) const {
      return new CLdsObjectType(iID, aProps, bStatic);
    };

    // Get value type
    virtual ELdsValueType GetType(void) const {
      return EVT_OBJECT;
    };

    // Type name
    virtual string TypeName(void) const {
      return "object";
    };

    // Clear the value
    virtual void Clear(void);

    // Value I/O
    virtual void Write(LdsEnginePtr pEngine, void *pStream);
    virtual void Read(LdsEnginePtr pEngine, void *pStream, CLdsValue &val);
  
  public:
    // Print the value
    virtual string Print(void);

    // Print one property
    string PrintProperty(const int &iProp);
    
    // Get variables
    virtual CLdsVars *GetVars(void) { return &aProps; };
    
    // Perform a unary operation
    virtual CLdsValueRef UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn);
    // Perform a binary operation
    virtual CLdsValueRef BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn);

    // Conditions
    virtual bool IsTrue(void) {
      return (iID != -1);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (iID == ((CLdsObjectType &)valOther).iID);
    };
};
