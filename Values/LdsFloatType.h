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

// Script float value
class LDS_API CLdsFloatType : public ILdsValueBase {
  public:
    double dValue; // float value

  public:
    // Default constructor
    CLdsFloatType(void) : dValue(0.0) {};

    // Float constructor
    CLdsFloatType(const double &d) : dValue(d) {};

    // Create new instance of this value
    virtual ILdsValueBase *MakeCopy(void) const {
      return new CLdsFloatType(dValue);
    };

    // Get value type
    virtual ELdsValueType GetType(void) const {
      return EVT_FLOAT;
    };
    
    // Type name
    virtual string TypeName(void) const {
      return "double";
    };

    // Clear the value
    virtual void Clear(void) {
      dValue = 0.0;
    };

    // Value I/O
    virtual void Write(LdsEnginePtr pEngine, void *pStream);
    virtual void Read(LdsEnginePtr pEngine, void *pStream, CLdsValue &val);
  
  public:
    // Print the value
    virtual string Print(void);
  
    // Get integer value
    virtual int GetIndex(void) { return (int)dValue; };
    // Get float value
    virtual double GetNumber(void) { return dValue; };
    
    // Perform a unary operation
    virtual CLdsValueRef UnaryOp(CLdsValueRef &valRef, const CLdsToken &tkn);
    // Perform a binary operation
    virtual CLdsValueRef BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn);

    // Conditions
    virtual bool IsTrue(void) {
      return (dValue >= 0.5);
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return (dValue == ((CLdsFloatType &)valOther).dValue);
    };
};
