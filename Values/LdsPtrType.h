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
#include "../Types/LdsVar.h"

// Script pointer value
class LDS_API CLdsPtrType : public ILdsValueBase {
  public:
    SLdsVar *pvarValue; // variable pointer

  public:
    // Default constructor
    CLdsPtrType(void) : pvarValue(NULL) {};

    // Constructor
    CLdsPtrType(SLdsVar *pvar);

    // Create new instance of this value
    virtual ILdsValueBase *MakeCopy(void) const {
      CLdsPtrType *pvalNew = new CLdsPtrType();
      pvalNew->pvarValue = pvarValue;

      return pvalNew;
    };

    // Get value type
    virtual ELdsValueType GetType(void) const {
      return EVT_POINTER;
    };

    // Clear the value
    virtual void Clear(void) {
      pvarValue = NULL;
    };
  
  public:
    // Type name
    virtual string TypeName(void) const { return "pointer"; };

    // Print the value
    virtual string Print(void);

    // Get pointer to the value's variable
    virtual SLdsVar *GetPointer(void) const { return pvarValue; };
  
    // Get integer value
    virtual int GetIndex(void) { return pvarValue->var_valValue->GetIndex(); };
    // Get float value
    virtual double GetNumber(void) { return pvarValue->var_valValue->GetNumber(); };
    // Get string value
    virtual string GetString(void) { return pvarValue->var_valValue->GetString(); };
    // Get array value
    virtual CLdsArray &GetArray(void) { return pvarValue->var_valValue->GetArray(); };
    // Get structure value
    virtual CLdsStruct &GetStruct(void) { return pvarValue->var_valValue->GetStruct(); };
    
    // Perform a unary operation
    virtual CLdsValueRef UnaryOp(CLdsValueRef &valRef, CCompAction &ca);
    // Perform a binary operation
    virtual CLdsValueRef BinaryOp(CLdsValueRef &valRef1, CLdsValueRef &valRef2, const CLdsToken &tkn);

    // Conditions
    virtual bool IsTrue(void) {
      return pvarValue != NULL;
    };
    
    virtual bool Compare(const ILdsValueBase &valOther) {
      return pvarValue == valOther.GetPointer();
    };
};
