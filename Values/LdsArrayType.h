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

// Script array value
class LDS_API CLdsArrayType : public ILdsValueBase {
  public:
    CLdsVars aArray; // array of values

  public:
    // Default constructor
    CLdsArrayType(void) {};

    // Array constructor
    CLdsArrayType(const int &ct, const CLdsValue &valDef) {
      aArray.aVars.New(ct);

      for (int i = 0; i < ct; i++) {
        aArray.aVars[i] = valDef;
      }
    };

    // Array copy constructor
    CLdsArrayType(const CLdsVars &a) : aArray(a) {};

    // Create new instance of this value
    virtual ILdsValueBase *MakeCopy(void) const {
      return new CLdsArrayType(aArray);
    };

    // Get value type
    virtual ELdsValueType GetType(void) const {
      return EVT_ARRAY;
    };

    // Type name
    virtual string TypeName(void) const {
      return "array";
    };

    // Clear the value
    virtual void Clear(void) {
      aArray.Clear();
    };

    // Value I/O
    virtual void Write(class CLdsScriptEngine *pEngine, void *pStream);
    virtual void Read(class CLdsScriptEngine *pEngine, void *pStream, CLdsValue &val);
  
  public:
    // Print the value
    virtual string Print(void);

    // Add array value
    inline int Add(const CLdsValue &val) {
      return aArray.Add(val);
    };

    // Get variables
    virtual CLdsVars *GetVars(void) { return &aArray; };
    
    // Perform a unary operation
    virtual CLdsValueRef UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn);
    // Perform a binary operation
    virtual CLdsValueRef BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn);

    // Conditions
    virtual bool IsTrue(void) {
      return (aArray.Count() > 0);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (aArray.Count() == ((CLdsArrayType &)valOther).aArray.Count());
    };
};
