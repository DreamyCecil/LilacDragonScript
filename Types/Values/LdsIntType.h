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
    TYPE_NAME_FUNC { return strNumber; };

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
